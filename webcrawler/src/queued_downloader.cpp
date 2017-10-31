#include "queued_downloader.h"
#include "constants.h"
#include "page_parser_helpers.h"

namespace WebCrawler
{

QueuedDownloader::QueuedDownloader()
	: AbstractThreadableObject(this, QByteArray("QueuedDownloader"))
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_unprocessedRepliesCount(0)
	, m_pendingRequestsCount(0)
{
	moveThisToSeparateThread();

	VERIFY(connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*))));
}

QueuedDownloader::~QueuedDownloader()
{
	stopExecution();
}

void QueuedDownloader::setUserAgent(const QByteArray& userAgent)
{
	m_userAgent = userAgent;
}

void QueuedDownloader::scheduleUrl(const CrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	m_requestQueue.push_back(url);
}

bool QueuedDownloader::extractReply(Reply& response) noexcept
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);
		
	if (m_repliesQueue.empty())
	{
		return false;
	}

	response = std::move(*m_repliesQueue.begin());
	m_repliesQueue.erase(m_repliesQueue.begin());

	--m_unprocessedRepliesCount;

	return true;
}

void QueuedDownloader::start() noexcept
{
	startExecution();
}

void QueuedDownloader::stop() noexcept
{
	stopExecution();
}

size_t QueuedDownloader::unprocessedRequestCount() const noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	return m_requestQueue.size();
}

void QueuedDownloader::urlDownloaded(QNetworkReply* reply)
{
	processReply(reply);
}

void QueuedDownloader::metaDataChanged(QNetworkReply* reply)
{
	if (isReplyProcessed(reply))
	{
		return;
	}

	const bool nonHtmlResponse = !PageParserHelpers::isHtmlContentType(
		reply->header(QNetworkRequest::ContentTypeHeader).toString()
	);

	if (nonHtmlResponse)
	{
		processReply(reply);
		
		reply->abort();
	}
}

void QueuedDownloader::queryError(QNetworkReply* reply, QNetworkReply::NetworkError code)
{
	ERRORLOG << "An error occurred while downloading " << reply->url().toDisplayString() << "; code: " << code;
}

void QueuedDownloader::processReply(QNetworkReply* reply)
{
	if (isReplyProcessed(reply))
	{
		return;
	}

	markReplyAsProcessed(reply);
	reply->disconnect(this);

	const bool nonHtmlResponse = !PageParserHelpers::isHtmlContentType(reply->header(QNetworkRequest::ContentTypeHeader).toString());
	const bool processBody = !nonHtmlResponse;

	QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	Reply response{ statusCode.isValid() ? statusCode.toInt() : -1, reply->url() };
	response.responseHeaders = reply->rawHeaderPairs();

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		response.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
	}

	if (processBody)
	{
		response.responseBody = reply->readAll();
	}

	const QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (!redirectUrl.isEmpty())
	{
		response.redirectUrl = redirectUrl;
	}

	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	m_repliesQueue.push_back(std::move(response));

	--m_pendingRequestsCount;
	++m_unprocessedRepliesCount;

	reply->deleteLater();
}

void QueuedDownloader::markReplyAsProcessed(QNetworkReply* reply) const noexcept
{
	ASSERT(reply != nullptr);
	reply->setProperty("processed", true);
}

bool QueuedDownloader::isReplyProcessed(QNetworkReply* reply) const noexcept
{
	if (reply == nullptr)
	{
		// was already destroyed by deleteLater()
		return true;
	}

	const QVariant alreadyProcessed = reply->property("processed");
	return alreadyProcessed.isValid();
}

void QueuedDownloader::process()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	static constexpr auto s_maxUnprocessedReplies = 200;
	static constexpr auto s_maxPendingRequests = 50;
	static constexpr auto s_maxRequestsOneBatch = 100;

	int requestsThisBatch = 0;
	
	while (m_requestQueue.size() && requestsThisBatch < s_maxRequestsOneBatch &&
		m_pendingRequestsCount < s_maxPendingRequests && m_unprocessedRepliesCount < s_maxUnprocessedReplies)
	{
		const CrawlerRequest request = *m_requestQueue.begin();
		QNetworkReply* reply = nullptr;

		QNetworkRequest networkRequest(request.url);
		networkRequest.setRawHeader("User-Agent", m_userAgent);

		switch (request.requestType)
		{
			case RequestTypeGet:
			{
				reply = m_networkAccessManager->get(networkRequest);
				break;
			}
			case RequestTypeHead:
			{
				reply = m_networkAccessManager->head(networkRequest);
				break;
			}
			default:
			{
				ASSERT(!"Unsupported request type");
				break;
			}
		}

		auto requestIt = m_requestQueue.begin();

		VERIFY(connect(reply, &QNetworkReply::metaDataChanged, this, [this, reply]() { metaDataChanged(reply); }, Qt::QueuedConnection));

		using ErrorSignal = void (QNetworkReply::*)(QNetworkReply::NetworkError);

		VERIFY(connect(reply, static_cast<ErrorSignal>(&QNetworkReply::error), this, 
			[this, reply](QNetworkReply::NetworkError code) { queryError(reply, code); }, Qt::QueuedConnection));

		m_requestQueue.erase(m_requestQueue.begin());

		++m_pendingRequestsCount;
		++requestsThisBatch;
	}
}

}
#include "queued_downloader.h"
#include "constants.h"
#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

QueuedDownloader::QueuedDownloader()
	: AbstractThreadableObject(this, QByteArray("QueuedDownloaderThread"))
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

void QueuedDownloader::scheduleUrl(const WebCrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);
	//INFOLOG << "Url scheduled: " << url.toDisplayString();
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

	//INFOLOG << "Reply extracted: " << response.url.toDisplayString() << " " << QThread::currentThreadId();
	m_unprocessedRepliesCount--;

	return true;
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

	bool nonHtmlResponse = !PageRawParserHelpers::isHtmlContentType(reply->header(QNetworkRequest::ContentTypeHeader).toString());
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

	markReplyProcessed(reply);
	reply->disconnect(this);

	bool nonHtmlResponse = !PageRawParserHelpers::isHtmlContentType(reply->header(QNetworkRequest::ContentTypeHeader).toString());
	bool processBody = !nonHtmlResponse;
	

	QVariant alreadyProcessed = reply->property("processed");

	QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	Reply response{ statusCode.isValid() ? statusCode.toInt() : -1, reply->url() };

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		response.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
		response.responseHeaders[headerValuePair.first] = headerValuePair.second;
	}

	if (processBody)
	{
		response.responseBody = reply->readAll();
	}

	QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirectUrl.isEmpty())
	{
		response.redirectUrl = redirectUrl;
	}

	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	//INFOLOG << "Reply added: " << response.url.toDisplayString() << " " << QThread::currentThreadId();
	m_repliesQueue.push_back(std::move(response));

	m_pendingRequestsCount--;
	m_unprocessedRepliesCount++;

	reply->deleteLater();
}

void QueuedDownloader::markReplyProcessed(QNetworkReply* reply) const noexcept
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
	QVariant alreadyProcessed = reply->property("processed");
	return alreadyProcessed.isValid();
}

void QueuedDownloader::process()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	static const int s_maxUnprocessedReplies = 200;
	static const int s_maxPendingRequests = 50;
	static const int s_maxRequestsOneBatch = 100;

	int requestsThisBatch = 0;
	
	while (m_requestQueue.size() && requestsThisBatch < s_maxRequestsOneBatch &&
		m_pendingRequestsCount < s_maxPendingRequests && m_unprocessedRepliesCount < s_maxUnprocessedReplies)
	{
		WebCrawlerRequest request = *m_requestQueue.begin();
		QNetworkReply* reply = nullptr;

		switch (request.requestType)
		{
		case RequestTypeGet:
			reply = m_networkAccessManager->get(QNetworkRequest(request.url));
			break;
		case RequestTypeHead:
			reply = m_networkAccessManager->head(QNetworkRequest(request.url));
			break;
		default:
			ASSERT(!"Unsupported request type");
			break;
		}

		auto requestIt = m_requestQueue.begin();
		//INFOLOG << "Request started" << requestIt->url() << " " << QThread::currentThreadId();

		VERIFY(QObject::connect(
			reply, 
			&QNetworkReply::metaDataChanged, 
			this, 
			[this, reply]() { metaDataChanged(reply); }, 
			Qt::QueuedConnection
		));

		using ErrorSignal = void (QNetworkReply::*)(QNetworkReply::NetworkError);
		VERIFY(QObject::connect(
			reply,
			static_cast<ErrorSignal>(&QNetworkReply::error),
			this,
			[this, reply](QNetworkReply::NetworkError code) { queryError(reply, code); },
			Qt::QueuedConnection
		));

		m_requestQueue.erase(m_requestQueue.begin());

		m_pendingRequestsCount++;
		requestsThisBatch++;
	}
}

}
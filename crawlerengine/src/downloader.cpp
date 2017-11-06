#include "downloader.h"
#include "handler_registry.h"
#include "download_request.h"
#include "download_response.h"
#include "thread_queue.h"
#include "page_parser_helpers.h"

namespace CrawlerEngine
{

Downloader::Downloader()
	: QObject(nullptr)
	, m_networkAccessor(new QNetworkAccessManager(this))
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeDownload);

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*)), Qt::DirectConnection));
}

void Downloader::setUserAgent(const QByteArray& userAgent)
{
	ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");

	m_userAgent = userAgent;
}

void Downloader::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestTypeDownload);
	DownloadRequest* request = static_cast<DownloadRequest*>(requester->request());

	QNetworkReply* reply = nullptr;

	QNetworkRequest networkRequest(request->requestInfo.url);
	networkRequest.setAttribute(QNetworkRequest::User, DownloadRequestType::RequestTypeGet);
	networkRequest.setRawHeader("User-Agent", m_userAgent);

	switch (request->requestInfo.requestType)
	{
		case DownloadRequestType::RequestTypeGet:
		{
			reply = m_networkAccessor->get(networkRequest);
			break;
		}
		case DownloadRequestType::RequestTypeHead:
		{
			reply = m_networkAccessor->head(networkRequest);
			break;
		}
		default:
		{
			ASSERT(!"Unsupported request type");
			break;
		}
	}

	VERIFY(connect(reply, &QNetworkReply::metaDataChanged, this, [this, reply]() { metaDataChanged(reply); }, Qt::QueuedConnection));

	using ErrorSignal = void (QNetworkReply::*)(QNetworkReply::NetworkError);

	VERIFY(connect(reply, static_cast<ErrorSignal>(&QNetworkReply::error), this,
		[this, reply](QNetworkReply::NetworkError code) { queryError(reply, code); }, Qt::QueuedConnection));

	m_requesters[request->requestInfo] = requester;
}

void Downloader::stopRequestHandling(RequesterSharedPtr requester)
{
	requester;
}

QObject* Downloader::qobject()
{
	return this;
}

void Downloader::urlDownloaded(QNetworkReply* reply)
{
	processReply(reply);
}

void Downloader::metaDataChanged(QNetworkReply* reply)
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

void Downloader::queryError(QNetworkReply* reply, QNetworkReply::NetworkError code)
{
	ERRORLOG << "An error occurred while downloading " << reply->url().toDisplayString() << "; code: " << code;
}

void Downloader::processReply(QNetworkReply* reply)
{
	if (isReplyProcessed(reply))
	{
		return;
	}

	markReplyAsProcessed(reply);
	reply->disconnect(this);

	const bool nonHtmlResponse = !PageParserHelpers::isHtmlContentType(reply->header(QNetworkRequest::ContentTypeHeader).toString());
	const bool processBody = !nonHtmlResponse;

	const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	std::shared_ptr<DownloadResponse> response = std::make_shared<DownloadResponse>();

	response->statusCode = statusCode.isValid() ? statusCode.toInt() : -1;
	response->url = reply->url();
	response->responseHeaders.addHeaderValues(reply->rawHeaderPairs());;

	if (processBody)
	{
		response->responseBody = reply->readAll();
	}

	const QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (!redirectUrl.isEmpty())
	{
		response->redirectUrl = redirectUrl;
	}

	reply->deleteLater();

	const CrawlerRequest key{ response->url, static_cast<DownloadRequestType>(reply->attribute(QNetworkRequest::User).toInt()) };
	const auto requesterIterator = m_requesters.find(key);

	if (requesterIterator == m_requesters.end())
	{
		return;
	}

	const RequesterSharedPtr requester = m_requesters[key].lock();

	if (!requester)
	{
		return;
	}

	ThreadQueue::forThread(requester->thread())->postResponse(requester, response);
}

bool Downloader::isReplyProcessed(QNetworkReply* reply) const noexcept
{
	if (reply == nullptr)
	{
		// was already destroyed by deleteLater()
		return true;
	}

	const QVariant alreadyProcessed = reply->property("processed");

	return alreadyProcessed.isValid();
}

void Downloader::markReplyAsProcessed(QNetworkReply* reply) const noexcept
{
	ASSERT(reply != nullptr);

	reply->setProperty("processed", true);
}

}
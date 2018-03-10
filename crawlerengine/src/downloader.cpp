#include "downloader.h"
#include "handler_registry.h"
#include "download_request.h"
#include "download_response.h"
#include "thread_message_dispatcher.h"
#include "page_parser_helpers.h"
#include "status_code.h"
#include "hop.h"
#include "service_locator.h"
#include "inotification_service.h"
#include "random_interval_range_timer.h"

namespace CrawlerEngine
{

Downloader::Downloader()
	: QObject(nullptr)
	, m_networkAccessor(new QNetworkAccessManager(this))
	, m_randomIntervalRangeTimer(new RandomIntervalRangeTimer(this))
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeDownload);

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*)), Qt::DirectConnection));

	VERIFY(connect(m_networkAccessor, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
		this, SLOT(proxyAuthenticationRequiredSlot(const QNetworkProxy&, QAuthenticator*)), Qt::DirectConnection));

	VERIFY(connect(m_randomIntervalRangeTimer, &RandomIntervalRangeTimer::timerTicked, 
		this, &Downloader::onTimerTicked, Qt::DirectConnection));
}

void Downloader::setPauseRange(int from, int to)
{
	resetPauseRange();
	m_randomIntervalRangeTimer->setRange(from, to);
	m_randomIntervalRangeTimer->start();
}

void Downloader::resetPauseRange()
{
	DEBUG_ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_randomIntervalRangeTimer->reset();
}

void Downloader::setUserAgent(const QByteArray& userAgent)
{
	DEBUG_ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_userAgent = userAgent;
}

void Downloader::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser,
	const QString& proxyPassword)
{
	DEBUG_ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");

	QNetworkProxy proxy;
	proxy.setType(QNetworkProxy::HttpProxy);
	proxy.setHostName(proxyHostName);
	proxy.setPort(proxyPort);

	if(!(proxyUser.isEmpty() && proxyPassword.isEmpty()))
	{
		proxy.setUser(proxyUser);
		proxy.setPassword(proxyPassword);
	}

	m_networkAccessor->setProxy(proxy);
}

void Downloader::resetProxy()
{
	m_networkAccessor->setProxy(QNetworkProxy::DefaultProxy);
}

void Downloader::handleRequest(RequesterSharedPtr requester)
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	ASSERT(requester->request()->requestType() == RequestType::RequestTypeDownload);

	if (m_randomIntervalRangeTimer->isValid())
	{
		m_requesterQueue.push(std::move(requester));
	}
	else
	{
		load(requester);
	}
}

void Downloader::stopRequestHandling(RequesterSharedPtr requester)
{
	//
	// TODO: reset loading link
	//

	requester;
}

QObject* Downloader::qobject()
{
	return this;
}

void Downloader::onTimerTicked()
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	if (m_requesterQueue.empty())
	{
		return;
	}

	RequesterSharedPtr requester(std::move(m_requesterQueue.front()));

	m_requesterQueue.pop();

	load(requester);
}

void Downloader::proxyAuthenticationRequiredSlot(const QNetworkProxy&, QAuthenticator*) const
{
	ServiceLocator* serviceLocator = ServiceLocator::instance();

	ASSERT(serviceLocator->isRegistered<INotificationService>());

	serviceLocator->service<INotificationService>()->error(tr("Proxy error"), tr("Proxy authentication failed."));
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

	const bool nonHtmlResponse = !PageParserHelpers::isHtmlOrPlainContentType(
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
	ERRLOG << "An error occurred while downloading " << reply->url().toDisplayString() << "; code: " << code;
}

void Downloader::processReply(QNetworkReply* reply)
{
	class ReplyDeferredDeleterGuard final
	{
	public:
		ReplyDeferredDeleterGuard(QNetworkReply* reply)
			: m_reply(reply)
		{
		}

		~ReplyDeferredDeleterGuard()
		{
			m_reply->deleteLater();
		}

	private:
		QNetworkReply* m_reply;
	};

	if (isReplyProcessed(reply))
	{
		return;
	}

	ReplyDeferredDeleterGuard guard(reply);

	markReplyAsProcessed(reply);
	reply->disconnect(this);

	const bool processBody = PageParserHelpers::isHtmlOrPlainContentType(
		reply->header(QNetworkRequest::ContentTypeHeader).toString()
	);

	const DownloadRequestType requestType = static_cast<DownloadRequestType>(reply->property("crawlerRequestType").toInt());
	const Common::StatusCode statusCode = static_cast<Common::StatusCode>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
	const int requestId = reply->property("requestId").toInt();

	if (!m_requesters.contains(requestId))
	{
		m_responses.remove(requestId);
		return;
	}

	const RequesterSharedPtr requester = m_requesters[requestId].lock();

	if (!requester)
	{
		return;
	}

	if (!m_responses.contains(requestId))
	{
		m_responses[requestId] = std::make_shared<DownloadResponse>();
	}

	std::shared_ptr<DownloadResponse> response = m_responses[requestId];

	const QByteArray body = processBody ? reply->readAll() : QByteArray();
	Url redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (redirectUrl.isValid() && redirectUrl.isRelative())
	{
		redirectUrl = PageParserHelpers::resolveRelativeUrl(redirectUrl, reply->url());
	}

	if (statusCode == Common::StatusCode::MovedPermanently301 ||
		statusCode == Common::StatusCode::MovedTemporarily302)
	{
		const CrawlerRequest redirectKey{ redirectUrl, requestType };
		loadHelper(redirectKey, requestId);

		response->hopsChain.addHop(Hop{ reply->url(), redirectUrl, statusCode, body, reply->rawHeaderPairs() });
		
		return;
	}
	
	response->hopsChain.addHop(Hop(reply->url(), redirectUrl, statusCode, body, reply->rawHeaderPairs()));
	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

	m_responses.remove(requestId);
	m_requesters.remove(requestId);

	ASSERT(m_activeRequests.removeOne(requestId));
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

void Downloader::load(RequesterSharedPtr requester)
{
	DownloadRequest* request = static_cast<DownloadRequest*>(requester->request());

	const int requestId = loadHelper(request->requestInfo);

	m_requesters[requestId] = requester;
}

int Downloader::loadHelper(const CrawlerRequest& request, int parentRequestId)
{
	static int s_request_id = 0;
	QNetworkReply* reply = nullptr;
	QNetworkRequest networkRequest(request.url);
	networkRequest.setRawHeader("User-Agent", m_userAgent);

	switch (request.requestType)
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

	reply->setProperty("crawlerRequestType", static_cast<int>(request.requestType));
	
	const int resultRequestId = parentRequestId == -1 ? s_request_id : parentRequestId;
	reply->setProperty("requestId", resultRequestId);

	if (parentRequestId == -1)
	{
		m_activeRequests.push_back(resultRequestId);
	}

	++s_request_id;

	VERIFY(connect(reply, &QNetworkReply::metaDataChanged, this, [this, reply]() { metaDataChanged(reply); }, Qt::QueuedConnection));

	using ErrorSignal = void (QNetworkReply::*)(QNetworkReply::NetworkError);

	VERIFY(connect(reply, static_cast<ErrorSignal>(&QNetworkReply::error), this,
		[this, reply](QNetworkReply::NetworkError code) { queryError(reply, code); }, Qt::QueuedConnection));

	return resultRequestId;
}

}
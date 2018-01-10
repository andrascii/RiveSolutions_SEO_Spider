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

namespace CrawlerEngine
{

Downloader::Downloader()
	: QObject(nullptr)
	, m_networkAccessor(new QNetworkAccessManager(this))
	, m_randomIntervalRangeTimer(new Common::RandomIntervalRangeTimer(this))
	, m_uniquenessChecker(createUniquenessChecker())
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeDownload);

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*)), Qt::DirectConnection));

	VERIFY(connect(m_networkAccessor, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
		this, SLOT(proxyAuthenticationRequiredSlot(const QNetworkProxy&, QAuthenticator*)), Qt::DirectConnection));

	VERIFY(connect(m_randomIntervalRangeTimer, &Common::RandomIntervalRangeTimer::timerTicked, 
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
	ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_randomIntervalRangeTimer->reset();
}

void Downloader::setUserAgent(const QByteArray& userAgent)
{
	ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");
	m_userAgent = userAgent;
}

void Downloader::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser,
	const QString& proxyPassword)
{
	ASSERT(thread() == QThread::currentThread() && "This method should be called from the same thread");

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
	requester;
}

QObject* Downloader::qobject()
{
	return this;
}

void Downloader::onTimerTicked()
{
	if (m_requesterQueue.empty())
	{
		return;
	}

	RequesterSharedPtr requester = m_requesterQueue.back();
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

	const DownloadRequestType requestType = static_cast<DownloadRequestType>(reply->attribute(QNetworkRequest::User).toInt());
	const Common::StatusCode statusCode = static_cast<Common::StatusCode>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
	const CrawlerRequest key{ reply->url(), requestType };

	if (!m_requesters.contains(key))
	{
		m_responses.remove(key);
		return;
	}

	const RequesterSharedPtr requester = m_requesters[key].lock();

	if (!requester)
	{
		return;
	}

	if (!m_responses.contains(key))
	{
		m_responses[key] = std::make_shared<DownloadResponse>();
	}

	std::shared_ptr<DownloadResponse> response = m_responses[key];

	const QByteArray body = processBody ? reply->readAll() : QByteArray();
	Url redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (redirectUrl.isRelative())
	{
		redirectUrl = PageParserHelpers::resolveRelativeUrl(redirectUrl, reply->url());
	}

	if (statusCode == Common::StatusCode::MovedPermanently301 ||
		statusCode == Common::StatusCode::MovedTemporarily302)
	{
		const CrawlerRequest redirectKey{ redirectUrl, requestType };

		m_requesters[redirectKey] = m_requesters[key];
		m_responses[redirectKey] = response;

		m_requesters.remove(key);
		m_responses.remove(key);

		response->hopsChain.addHop(Hop{ reply->url(), redirectUrl, statusCode, body, reply->rawHeaderPairs() });
		loadHelper(redirectKey);
		return;
	}
	
	response->hopsChain.addHop(Hop(reply->url(), redirectUrl, statusCode, body, reply->rawHeaderPairs()));
	ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
	m_responses.remove(key);
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

	ASSERT(!m_uniquenessChecker->hasRequest(request->requestInfo));
	m_uniquenessChecker->registrateRequest(request->requestInfo);

	loadHelper(request->requestInfo);

	m_requesters[request->requestInfo] = requester;
}

void Downloader::loadHelper(const CrawlerRequest& request)
{
	QNetworkReply* reply = nullptr;
	QNetworkRequest networkRequest(request.url);
	networkRequest.setAttribute(QNetworkRequest::User, static_cast<int>(DownloadRequestType::RequestTypeGet));
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
			DEBUG_ASSERT(!"Unsupported request type");
			break;
		}
	}

	VERIFY(connect(reply, &QNetworkReply::metaDataChanged, this, [this, reply]() { metaDataChanged(reply); }, Qt::QueuedConnection));

	using ErrorSignal = void (QNetworkReply::*)(QNetworkReply::NetworkError);

	VERIFY(connect(reply, static_cast<ErrorSignal>(&QNetworkReply::error), this,
		[this, reply](QNetworkReply::NetworkError code) { queryError(reply, code); }, Qt::QueuedConnection));
}

}
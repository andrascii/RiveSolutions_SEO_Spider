#include "host_info_provider.h"
#include "handler_registry.h"
#include "host_info.h"
#include "get_host_info_request.h"
#include "get_host_info_response.h"
#include "thread_message_dispatcher.h"
#include "download_response.h"
#include "crawler_request.h"
#include "download_request.h"
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

HostInfoProvider::HostInfoProvider()
	: QObject(nullptr)
{
#ifdef Q_OS_WIN

	int error = WSAStartup(MAKEWORD(2, 2), &m_wsadata);

	if (error != 0)
	{
		DEBUGLOG << error;

		DEBUG_ASSERT(!"WSAStartup failed with error");
	}

#endif

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestGetHostInfo);
}

HostInfoProvider::~HostInfoProvider()
{
#ifdef Q_OS_WIN

	WSACleanup();

#endif

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void HostInfoProvider::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestGetHostInfo);
	m_requester = requester;
	GetHostInfoRequest* request = static_cast<GetHostInfoRequest*>(requester->request());
	m_pendingResponse.reset(new GetHostInfoResponse(HostInfo(request->webpage.host(QUrl::FullyEncoded).toLatin1())));

	const DownloadRequest downloadRequest(CrawlerRequest{ request->webpage, DownloadRequestType::RequestTypeHead}, CrawlerSharedState::instance()->turnaround());
	m_downloadRequester.reset(downloadRequest, this, &HostInfoProvider::onLoadingDone);
	m_downloadRequester->start();

	DEBUGLOG << "IP address for:" << request->webpage.toDisplayString()
		<< "is" << (!m_pendingResponse->hostInfo.stringAddressesIPv4().isEmpty() ? m_pendingResponse->hostInfo.stringAddressesIPv4()[0] : QByteArray("unknown"));
}

void HostInfoProvider::stopRequestHandling(RequesterSharedPtr requester)
{
	requester;
}

void HostInfoProvider::onLoadingDone(Requester*, const DownloadResponse& response)
{
	m_pendingResponse->url = response.hopsChain.firstHop().url();

	if (response.hopsChain.length() > 1)
	{
		const Hop& hop = response.hopsChain.lastHop();
		const QUrl& originalUrl = m_pendingResponse->url;
		const QUrl& redirectedUrl = hop.url();

		const QString originalHost = originalUrl.host().toLower();
		const QString redirectedHost = redirectedUrl.host().toLower();

		if ((QString("www.") + originalHost) == redirectedHost ||
			(QString("www.") + redirectedHost) == originalHost)
		{
			const QString originalPath = originalUrl.path().toLower();
			const QString redirectedPath = redirectedUrl.path().toLower();

			if (originalPath == redirectedPath ||
				redirectedPath == QString("/") && originalPath.isEmpty() ||
				originalPath == QString("/") && redirectedPath.isEmpty())
			{
				m_pendingResponse->url = redirectedUrl;
			}
		}
	}

	ThreadMessageDispatcher::forThread(m_requester->thread())->postResponse(m_requester, m_pendingResponse);
	m_pendingResponse.reset();
	m_downloadRequester.reset();
	m_requester.reset();
}

QObject* HostInfoProvider::qobject()
{
	return this;
}

}

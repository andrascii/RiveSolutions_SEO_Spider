#include "proper_404_checker.h"
#include "handler_registry.h"
#include "crawler_request.h"
#include "download_request.h"
#include "download_response.h"
#include "thread_message_dispatcher.h"

namespace CrawlerEngine
{

Proper404Checker::Proper404Checker()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestCheck404IsProper);
}

Proper404Checker::~Proper404Checker()
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void Proper404Checker::handleRequest(RequesterSharedPtr requester)
{
	m_requester = requester;
	Check404IsProperRequest* request = static_cast<Check404IsProperRequest*>(requester->request());
	
	Url url = request->url;
	url.setPath(QString("/this-path-should-never-be-presented-on-this-web-site"));
	DownloadRequest downloadRequest(CrawlerRequest{url , DownloadRequestType::RequestTypeHead });
	m_downloadRequester.reset(downloadRequest, this, &Proper404Checker::onLoadingDone);
	m_downloadRequester->start();
}

void Proper404Checker::stopRequestHandling(RequesterSharedPtr requester)
{
	m_downloadRequester.reset();
	m_requester.reset();
}

QObject* Proper404Checker::qobject()
{
	return this;
}

void Proper404Checker::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	Q_UNUSED(requester);
	Common::StatusCode resultStatusCode = response.hopsChain.firstHop().statusCode();
	const bool success = Common::StatusCode::NotFound404 == resultStatusCode;
	IResponseSharedPtr resultResponse = std::make_shared<Check404IsProperResponse>(success);
	ThreadMessageDispatcher::forThread(m_requester->thread())->postResponse(m_requester, resultResponse);

	m_downloadRequester.reset();
	m_requester.reset();
}

}

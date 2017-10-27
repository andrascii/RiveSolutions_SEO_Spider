#include "loader.h"
#include "load_response.h"
#include "load_request.h"

namespace Common
{

Loader::Loader(QObject* parent)
	: QObject(parent)
{
	QThread* loaderThread = new QThread;
	moveToThread(loaderThread);
	loaderThread->start();

	m_networkAccessor = new QNetworkAccessManager(this);

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTypeLoad);

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*))));
}


Loader::Loader(const Loader& other)
	: m_networkAccessor(other.m_networkAccessor)
	, m_requester(other.m_requester)
{
}

void Loader::handleRequest(RequesterSharedPtr requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestTypeLoad);

	const LoadRequest* loadRequest = static_cast<const LoadRequest*>(requester->request());
	m_requester = requester;

	m_networkAccessor->get(QNetworkRequest(loadRequest->url()));
}

void Loader::stopRequestHandling(RequesterSharedPtr requester)
{

}

void Loader::onLoadingDone(QNetworkReply* reply)
{
	std::shared_ptr<LoadResponse> loadResponse = std::make_shared<LoadResponse>();
	loadResponse->setBody(reply->readAll());

	ThreadQueue::forThread(m_requester->thread())->postResponse(m_requester, loadResponse);
}

}
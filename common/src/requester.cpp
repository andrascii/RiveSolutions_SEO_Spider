#include "requester.h"
#include "handler_registry.h"
#include "thread_queue.h"

namespace Common
{

Requester::Requester(const IRequest& request)
	: m_request(request.clone())
{
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	m_handler = handlerRegistry.handlerForRequest(request);

	ASSERT(m_handler && "Handler for this request was not registered");

	qRegisterMetaType<Requester>("Requester");
	qRegisterMetaType<RequesterSharedPtr>("RequesterSharedPtr");
}

Requester::Requester(const Requester& other)
	: m_request(other.m_request)
	, m_delegate(other.m_delegate)
	, m_handler(other.m_handler)
{
}

QObject* Requester::handler() const noexcept
{
	return m_handler;
}

IRequest* Requester::request() const noexcept
{
	return m_request.get();
}

void Requester::start()
{
	ThreadQueue::forCurrentThread()->startRequester(shared_from_this());
}

void Requester::stop()
{
	ThreadQueue::forCurrentThread()->stopRequester(shared_from_this());
}

void Requester::processResponse(const IResponse& response) const
{
	m_delegate(response);
}

}
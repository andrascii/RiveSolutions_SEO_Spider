#include "message.h"
#include "requester.h"

namespace Common
{

Message Message::undefinedMessage()
{
	return Message(Message::MessageTypeUndefined, QThread::currentThread(), nullptr, nullptr);
}

Message Message::startRequestMessage(RequesterSharedPtr requester)
{
	return Message(MessageTypeStartRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
}

Message Message::stopRequestMessage(RequesterSharedPtr requester)
{
	return Message(MessageTypeStopRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
}

Message Message::postResponseMessage(RequesterSharedPtr requester, IResponseSharedPtr response)
{
	return Message(MessageTypePostResponse, requester->thread(), requester, response);
}

Message::MessageType Message::type() const noexcept
{
	return m_type;
}

QThread* Message::targetThread() const noexcept
{
	return m_targetThread;
}

Requester* Message::requester() const noexcept
{
	return m_requester.get();
}

IResponse* Message::response() const noexcept
{
	return m_response.get();
}

IRequest* Message::request() const noexcept
{
	return requester()->request();
}

QObject* Message::handler() const noexcept
{
	return requester()->handler();
}

Message::Message(MessageType type, QThread* targetThread, RequesterSharedPtr requester, IResponseSharedPtr response)
	: m_type(type)
	, m_targetThread(targetThread)
	, m_requester(requester)
	, m_response(response)
{
}

}
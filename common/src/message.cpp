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
	if (requester)
	{
		return Message(MessageTypeStartRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
	}

	return undefinedMessage();
}

Message Message::stopRequestMessage(RequesterSharedPtr requester)
{
	if (requester)
	{
		return Message(MessageTypeStopRequest, requester->handler()->thread(), requester, IResponseSharedPtr());
	}

	return undefinedMessage();
}

Message Message::postResponseMessage(RequesterSharedPtr requester, IResponseSharedPtr response)
{
	if (requester)
	{
		return Message(MessageTypePostResponse, requester->thread(), requester, response);
	}
	
	return undefinedMessage();
}

Message::MessageType Message::type() const noexcept
{
	return m_type;
}

QThread* Message::targetThread() const noexcept
{
	return m_targetThread;
}

RequesterSharedPtr Message::requester() const noexcept
{
	return m_requester.lock();
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
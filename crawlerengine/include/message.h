#pragma once

#include "iresponse.h"
#include "requester.h"

namespace CrawlerEngine
{

class IRequest;

class Message
{
public:
	enum MessageType
	{
		MessageTypeUndefined,
		MessageTypeStartRequest,
		MessageTypeStopRequest,
		MessageTypePostResponse
	};

	static Message undefinedMessage();
	static Message startRequestMessage(RequesterSharedPtr requester);
	static Message stopRequestMessage(RequesterSharedPtr requester);
	static Message postResponseMessage(RequesterSharedPtr requester, IResponseSharedPtr response);

	MessageType type() const noexcept;
	QThread* targetThread() const noexcept;
	RequesterSharedPtr requester() const noexcept;
	IResponse* response() const noexcept;
	IRequest* request() const noexcept;
	QObject* handler() const noexcept;

private:
	Message(MessageType type, QThread* targetThread, RequesterSharedPtr requester, IResponseSharedPtr response);

private:
	MessageType m_type;
	QThread* m_targetThread;
	RequesterWeakPtr m_requester;
	IResponseSharedPtr m_response;
};

}

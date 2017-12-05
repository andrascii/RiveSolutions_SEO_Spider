#pragma once

#include "message_queue.h"

namespace CrawlerEngine
{

struct IResponse;
class Requester;

class ThreadMessageDispatcher : public QObject
{
	Q_OBJECT

public:
	static std::shared_ptr<ThreadMessageDispatcher> forThread(QThread* thread);
	static std::shared_ptr<ThreadMessageDispatcher> forCurrentThread();

	void startRequester(RequesterSharedPtr requester);
	void stopRequester(RequesterSharedPtr requester);
	void postResponse(RequesterSharedPtr requester, IResponseSharedPtr response);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	ThreadMessageDispatcher(QThread* thread);

	Q_SLOT void shutdown();

	void startDispatchTimer();
	void stopDispatchTimer();

	void execute();

	MessageQueue& messageQueue() noexcept;

private:
	int m_dispatchTimerId;

	MessageQueue m_messageQueue;
};

}
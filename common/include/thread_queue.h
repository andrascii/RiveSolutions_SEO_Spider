#pragma once

#include "message_queue.h"

namespace Common
{

struct IResponse;
class Requester;

class ThreadQueue : public QObject
{
	Q_OBJECT

public:
	static std::shared_ptr<ThreadQueue> forThread(QThread* thread);
	static std::shared_ptr<ThreadQueue> forCurrentThread();

	void startRequester(RequesterSharedPtr requester);
	void stopRequester(RequesterSharedPtr requester);
	void postResponse(RequesterSharedPtr requester, IResponseSharedPtr response);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	ThreadQueue(QThread* thread);

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
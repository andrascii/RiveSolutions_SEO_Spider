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

	void startRequest(RequesterSharedPtr requester);
	void stopRequest(RequesterSharedPtr requester);
	void postResponse(RequesterSharedPtr requester, IResponseSharedPtr response);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	ThreadMessageDispatcher(QThread* thread);

	void startDispatchTimer();
	void stopDispatchTimer();

	void execute();

	MessageQueue& messageQueue() noexcept;

private slots:
	void shutdown();

private:
	int m_dispatchTimerId;

	MessageQueue m_messageQueue;
};

}
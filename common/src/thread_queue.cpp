#include "thread_queue.h"
#include "common_macro_helpers.h"
#include "constants.h"
#include "requester.h"
#include "handler_registry.h"

namespace Common
{

struct ThreadMetaData : public QObjectUserData
{
	ThreadMetaData()
	{
	}

	ThreadMetaData(std::shared_ptr<ThreadQueue> queue)
		: queue(queue)
	{
	}

	std::shared_ptr<ThreadQueue> queue;
};

}

namespace
{

using namespace Common;

int metaDataIndex()
{
	static int s_metaDataIndex = QObject::registerUserData();
	return s_metaDataIndex;
}

void registerThreadQueue(QThread* thread, std::shared_ptr<ThreadQueue> controller)
{
	thread->setUserData(metaDataIndex(), new ThreadMetaData(controller));
}

void unregisterThreadQueue(QThread* thread)
{
	delete thread->userData(metaDataIndex());

	thread->setUserData(metaDataIndex(), nullptr);
}

std::shared_ptr<ThreadQueue> threadQueue(QThread* thread)
{
	QObjectUserData* metaData = thread->userData(metaDataIndex());

	if (metaData == nullptr)
	{
		return std::shared_ptr<ThreadQueue>();
	}

	return static_cast<ThreadMetaData*>(metaData)->queue;
}

std::mutex s_mutex;

}

namespace Common
{

std::shared_ptr<ThreadQueue> ThreadQueue::forThread(QThread* thread)
{
	std::lock_guard<std::mutex> locker(s_mutex);

	std::shared_ptr<ThreadQueue> threadQueuePtr = threadQueue(thread);

	if (!threadQueuePtr)
	{
		threadQueuePtr.reset(new ThreadQueue(thread));
		registerThreadQueue(thread, threadQueuePtr);
	}

	return threadQueuePtr;
}

std::shared_ptr<ThreadQueue> ThreadQueue::forCurrentThread()
{
	return ThreadQueue::forThread(QThread::currentThread());
}

void ThreadQueue::startRequester(RequesterSharedPtr requester)
{
	messageQueue().addMessage(Message::startRequestMessage(requester));
}

void ThreadQueue::stopRequester(RequesterSharedPtr requester)
{
	messageQueue().addMessage(Message::stopRequestMessage(requester));
}

void ThreadQueue::postResponse(RequesterSharedPtr requester, IResponseSharedPtr response)
{
	messageQueue().addMessage(Message::postResponseMessage(requester, response));
}

ThreadQueue::ThreadQueue(QThread* thread)
{
	startDispatchTimer();
	moveToThread(thread);
	VERIFY(connect(thread, SIGNAL(finished()), this, SLOT(shutdown()), Qt::DirectConnection)); 
}

Q_SLOT void ThreadQueue::shutdown()
{
	stopDispatchTimer();
	unregisterThreadQueue(thread());
}

void ThreadQueue::startDispatchTimer()
{
	m_dispatchTimerId = startTimer(Common::g_minimumRecommendedTimerResolution);
	ASSERT(m_dispatchTimerId);
}

void ThreadQueue::stopDispatchTimer()
{
	ASSERT(m_dispatchTimerId);
	killTimer(m_dispatchTimerId);
}

void ThreadQueue::execute()
{
	if (messageQueue().isEmpty())
	{
		return;
	}

	Message message = messageQueue().extractMessage();
	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	RequesterSharedPtr requester = message.requester();

	if (!requester || requester->thread() != thread() || !handlerRegistry.isHandlerExists(message.handler()))
	{
		return;
	}

	switch(message.type())
	{
		case Message::MessageTypeStartRequest:
		{
			VERIFY(QMetaObject::invokeMethod(message.handler(), "handleRequest", 
				Qt::QueuedConnection, Q_ARG(RequesterSharedPtr, requester)));

			break;
		}
		case Message::MessageTypeStopRequest:
		{
			VERIFY(QMetaObject::invokeMethod(message.handler(), "stopRequestHandling",
				Qt::QueuedConnection, Q_ARG(RequesterSharedPtr, requester)));

			break;
		}
		case Message::MessageTypePostResponse:
		{
			requester->processResponse(*message.response());

			break;
		}
	}
}

MessageQueue& ThreadQueue::messageQueue() noexcept
{
	return m_messageQueue;
}

void ThreadQueue::timerEvent(QTimerEvent* event)
{
	execute();
}

}

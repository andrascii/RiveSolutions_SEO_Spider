#include "stdafx.h"
#include "thread_message_dispatcher.h"
#include "common_macro_helpers.h"
#include "common_constants.h"
#include "requester.h"
#include "handler_registry.h"

namespace CrawlerEngine
{

struct ThreadMetaData : public QObjectUserData
{
	ThreadMetaData()
	{
	}

	ThreadMetaData(std::shared_ptr<ThreadMessageDispatcher> dispatcher)
		: dispatcher(dispatcher)
	{
	}

	std::shared_ptr<ThreadMessageDispatcher> dispatcher;
};

}

namespace
{

using namespace CrawlerEngine;

int metaDataIndex()
{
	static int s_metaDataIndex = QObject::registerUserData();
	return s_metaDataIndex;
}

void registerThreadQueue(QThread* thread, std::shared_ptr<ThreadMessageDispatcher> dispatcher)
{
	thread->setUserData(metaDataIndex(), new ThreadMetaData(dispatcher));
}

void unregisterThreadQueue(QThread* thread)
{
	delete thread->userData(metaDataIndex());

	thread->setUserData(metaDataIndex(), nullptr);
}

std::shared_ptr<ThreadMessageDispatcher> threadQueue(QThread* thread)
{
	QObjectUserData* metaData = thread->userData(metaDataIndex());

	if (metaData == nullptr)
	{
		return std::shared_ptr<ThreadMessageDispatcher>();
	}

	return static_cast<ThreadMetaData*>(metaData)->dispatcher;
}

std::mutex s_mutex;

}

namespace CrawlerEngine
{

std::shared_ptr<ThreadMessageDispatcher> ThreadMessageDispatcher::forThread(QThread* thread)
{
	std::lock_guard locker(s_mutex);

	std::shared_ptr<ThreadMessageDispatcher> threadQueuePtr = threadQueue(thread);

	if (!threadQueuePtr)
	{
		threadQueuePtr.reset(new ThreadMessageDispatcher(thread));
		registerThreadQueue(thread, threadQueuePtr);
	}

	return threadQueuePtr;
}

std::shared_ptr<ThreadMessageDispatcher> ThreadMessageDispatcher::forCurrentThread()
{
	return ThreadMessageDispatcher::forThread(QThread::currentThread());
}

void ThreadMessageDispatcher::startRequest(RequesterSharedPtr requester)
{
	messageQueue().addMessage(Message::startRequestMessage(requester));
}

void ThreadMessageDispatcher::stopRequest(RequesterSharedPtr requester)
{
	messageQueue().addMessage(Message::stopRequestMessage(requester));
}

void ThreadMessageDispatcher::postResponse(RequesterSharedPtr requester, IResponseSharedPtr response)
{
	messageQueue().addMessage(Message::postResponseMessage(requester, response));
}

ThreadMessageDispatcher::ThreadMessageDispatcher(QThread* thread)
	: m_dispatchTimerId(0)
{
	moveToThread(thread);
	startDispatchTimer();

	VERIFY(connect(thread, SIGNAL(finished()), this, SLOT(shutdown()), Qt::DirectConnection));

	if (thread == qApp->thread())
	{
		VERIFY(connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()), Qt::DirectConnection));
	}
}

void ThreadMessageDispatcher::shutdown()
{
	stopDispatchTimer();
	unregisterThreadQueue(thread());
}

void ThreadMessageDispatcher::startDispatchTimer()
{
	m_dispatchTimerId = startTimer(Common::c_minimumRecommendedTimerResolution);
	ASSERT(m_dispatchTimerId);
}

void ThreadMessageDispatcher::stopDispatchTimer()
{
	ASSERT(m_dispatchTimerId);
	killTimer(m_dispatchTimerId);
}

void ThreadMessageDispatcher::execute()
{
	while (!messageQueue().isEmpty())
	{
		Message message = std::move(messageQueue().extractMessage());
		HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
		RequesterSharedPtr requester = message.requester();

		if (!requester)
		{
			WARNLOG << "Requester is dead";
			continue;
		}

		if (!handlerRegistry.isHandlerExists(message.handler()))
		{
			WARNLOG << "Handler for request type is not exists";
			continue;
		}

		switch (message.type())
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
				ASSERT(requester->thread() == thread());

				requester->processResponse(*message.response());

				break;
			}
		}
	}
}

MessageQueue& ThreadMessageDispatcher::messageQueue() noexcept
{
	return m_messageQueue;
}

void ThreadMessageDispatcher::timerEvent(QTimerEvent*)
{
	execute();
}

}

#include "abstract_threadable_object.h"
#include "constants.h"

namespace QuickieWebBot
{

AbstractThreadableObject::~AbstractThreadableObject()
{
	m_thread.exit(0);
}

void AbstractThreadableObject::moveThisToSeparateThread()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	m_derivedObjectPtr->moveToThread(&m_thread);
}

bool AbstractThreadableObject::isRunning() const noexcept
{
	return m_isRunning.load();
}

void AbstractThreadableObject::startExecution() noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!m_thread.isRunning())
	{
		m_thread.start();

		QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
			Q_ARG(int, g_minimumRecommendedTimerResolution), Q_ARG(Qt::TimerType, Qt::CoarseTimer));

		m_isRunning.store(true);
	}
}

void AbstractThreadableObject::stopExecution() noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (m_thread.isRunning())
	{
		QMetaObject::invokeMethod(this, "killTimer",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_timerId));

		m_thread.quit();
		m_thread.wait();

		m_isRunning.store(false);
	}
}

void AbstractThreadableObject::waitExecution() noexcept
{
	m_thread.wait();
}

const QByteArray& AbstractThreadableObject::threadName() const
{
	return m_threadName;
}

void AbstractThreadableObject::timerEvent(QTimerEvent* event)
{
	process();
}

void AbstractThreadableObject::startTimer(int interval, Qt::TimerType timerType)
{
#ifdef Q_OS_WIN

	INFOLOG << threadName() << QThread::currentThreadId() << "Thread has been started";

#endif

	m_timerId = QObject::startTimer(g_minimumRecommendedTimerResolution);
	assert(m_timerId);
}

void AbstractThreadableObject::killTimer(int timerId)
{
	if (m_timerId)
	{
		QObject::killTimer(timerId);
		m_timerId = 0;
	}
}

}
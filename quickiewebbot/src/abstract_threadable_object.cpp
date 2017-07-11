#include "abstract_threadable_object.h"
#include "constants.h"

namespace QuickieWebBot
{

AbstractThreadableObject::AbstractThreadableObject()
	: m_isRunning(false)
	, m_timerId(0)
{
}

void AbstractThreadableObject::moveThisToSeparateThread()
{
	moveToThread(&m_thread);
}

bool AbstractThreadableObject::isRunning() const noexcept
{
	return m_isRunning.load();
}

void AbstractThreadableObject::start() noexcept
{
	if (!m_thread.isRunning())
	{
		m_thread.start();

		QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
			Q_ARG(int, g_minimumRecommendedTimerResolution), Q_ARG(Qt::TimerType, Qt::CoarseTimer));

		m_isRunning.store(true);
	}
}

void AbstractThreadableObject::stop() noexcept
{
	if (m_thread.isRunning())
	{
		QMetaObject::invokeMethod(this, "killTimer",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_timerId));

		m_thread.quit();

		m_isRunning.store(false);
	}
}

void AbstractThreadableObject::timerEvent(QTimerEvent* event)
{
	process();
}

void AbstractThreadableObject::startTimer(int interval, Qt::TimerType timerType)
{
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
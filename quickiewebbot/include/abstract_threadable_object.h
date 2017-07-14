#pragma once

#include "application.h"

namespace QuickieWebBot
{

class AbstractThreadableObject : public QObject
{
	Q_OBJECT

public:
	template <typename Derived>
	AbstractThreadableObject(Derived* derivedObjectPtr)
		: m_derivedObjectPtr(derivedObjectPtr)
		, m_isRunning(false)
		, m_timerId(0)
	{
		static_assert(std::is_base_of<AbstractThreadableObject, Derived>::value,
			"Passed type must be derived from this class type.");

		assert(theApp->thread() == QObject::thread());
	}

	virtual ~AbstractThreadableObject();

	void moveThisToSeparateThread();

	// all methods below assumed to be thread-safe!

	bool isRunning() const noexcept;

	void startExecution() noexcept;
	void stopExecution() noexcept;
	void waitExecution() noexcept;

protected:
	virtual void process() = 0;

	virtual void timerEvent(QTimerEvent* event) override;

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	QObject* m_derivedObjectPtr;

	QThread m_thread;

	std::atomic_bool m_isRunning;

	int m_timerId;

	mutable std::mutex m_mutex;
};

}
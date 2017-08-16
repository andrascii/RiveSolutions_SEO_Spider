#pragma once

//#include "application.h"
#include "named_thread.h"

namespace WebCrawler
{

class AbstractThreadableObject : public QObject
{
	Q_OBJECT

public:
	template <typename Derived>
	AbstractThreadableObject(Derived* derivedObjectPtr, const QByteArray& threadName = QByteArray())
		: m_derivedObjectPtr(derivedObjectPtr)
		, m_threadName(threadName)
		, m_thread(m_threadName)
		, m_isRunning(false)
		, m_timerId(0)
	{
		static_assert(std::is_base_of<AbstractThreadableObject, Derived>::value,
			"Passed type must be derived from this class type.");

		assert(QApplication::instance()->thread() == QObject::thread());
	}

	virtual ~AbstractThreadableObject();

	void moveThisToSeparateThread();

	// all methods below assumed to be thread-safe!

	bool isRunning() const noexcept;

	void startExecution() noexcept;
	void stopExecution() noexcept;
	void waitExecution() noexcept;

	const QByteArray& threadName() const;

protected:
	virtual void process() = 0;

	virtual void timerEvent(QTimerEvent* event) override;

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	QObject* m_derivedObjectPtr;

	QByteArray m_threadName;

	NamedThread m_thread;

	std::atomic_bool m_isRunning;

	int m_timerId;

	mutable std::mutex m_mutex;
};

}
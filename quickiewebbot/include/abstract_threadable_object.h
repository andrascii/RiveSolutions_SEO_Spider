#pragma once

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
	}

	virtual ~AbstractThreadableObject() = default;

	template <typename Derived>
	void moveThisToSeparateThread()
	{
		static_assert(std::is_base_of<AbstractThreadableObject, Derived>::value, 
			"Passed type must be derived from this class type.");

		Derived* derived = static_cast<Derived* const>(this);

		assert(derived == m_derivedObjectPtr);

		derived->moveToThread(&m_thread);
	}

	bool isRunning() const noexcept;

	void start() noexcept;
	void stop() noexcept;

protected:
	virtual void process() = 0;

	virtual void timerEvent(QTimerEvent* event) override;

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	void* m_derivedObjectPtr;

	QThread m_thread;

	std::atomic_bool m_isRunning;

	int m_timerId;
};

}
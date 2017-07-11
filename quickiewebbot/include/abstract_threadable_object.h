#pragma once

namespace QuickieWebBot
{

class AbstractThreadableObject : public QObject
{
	Q_OBJECT

public:
	AbstractThreadableObject();
	virtual ~AbstractThreadableObject() = default;

	void moveThisToSeparateThread();
	bool isRunning() const noexcept;

	void start() noexcept;
	void stop() noexcept;

protected:
	virtual void process() = 0;

	virtual void timerEvent(QTimerEvent* event) override;

	Q_INVOKABLE void startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
	Q_INVOKABLE void killTimer(int timerId);

private:
	QThread m_thread;

	std::atomic_bool m_isRunning;

	int m_timerId;
};

}
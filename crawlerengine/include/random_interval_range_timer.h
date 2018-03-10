#pragma once

namespace CrawlerEngine
{

//
// From and To in milliseconds
//

class RandomIntervalRangeTimer : public QObject
{
	Q_OBJECT

public:
	RandomIntervalRangeTimer(QObject* parent = nullptr);
	RandomIntervalRangeTimer(int from, int to, QObject* parent = nullptr);

	bool isValid() const noexcept;
	void setRange(int from, int to) noexcept;
	void reset() noexcept;
	void start();
	void stop();

signals:
	void timerTicked();

private slots:
	void onTimerTicked();

private:
	int valueInRange() const noexcept;

private:
	int m_from;
	int m_to;
	bool m_started;
	QTimer* m_timer;
};

}
#include "random_interval_range_timer.h"

namespace CrawlerEngine
{

RandomIntervalRangeTimer::RandomIntervalRangeTimer(QObject* parent)
	: RandomIntervalRangeTimer(-1, -1, parent)
{
}

RandomIntervalRangeTimer::RandomIntervalRangeTimer(int from, int to, QObject* parent)
	: QObject(parent)
	, m_from(from)
	, m_to(to)
	, m_started(false)
	, m_timer(new QTimer(this))
{
	VERIFY(connect(m_timer, &QTimer::timeout, this, &RandomIntervalRangeTimer::onTimerTicked));
	//connect(m_timer, &QTimer::timeout, this, &RandomIntervalRangeTimer::onTimerTicked);
}

bool RandomIntervalRangeTimer::isValid() const noexcept
{
	return m_from != -1 && m_to != -1;
}

void RandomIntervalRangeTimer::setRange(int from, int to) noexcept
{
	if (from && to)
	{
		m_from = from;
		m_to = to;

		return;
	}

	reset();
}

void RandomIntervalRangeTimer::reset() noexcept
{
	stop();
	setRange(-1, -1);
}

void RandomIntervalRangeTimer::start()
{
	m_started = true;
	m_timer->setSingleShot(true);
	m_timer->start(valueInRange());
}

void RandomIntervalRangeTimer::stop()
{
	m_started = false;
	m_timer->stop();
}

int RandomIntervalRangeTimer::valueInRange() const noexcept
{
	return (qrand() % (m_to + 1 - m_from)) + m_from;
}

void RandomIntervalRangeTimer::onTimerTicked()
{
	emit timerTicked();

	start();
}

}
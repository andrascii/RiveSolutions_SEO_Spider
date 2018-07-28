#pragma once

#include "istatistic_counter.h"
#include "command.h"

namespace SeoSpider
{

class StatisticCounter : public QObject, public IStatisticCounter
{
	Q_OBJECT

public:
	StatisticCounter(const QString& counterName, QObject* parent = nullptr);
	virtual ~StatisticCounter();

	virtual const QString& name() const override;
	virtual CounterValue value() const override;
	virtual void increment() override;
	virtual void commit() override;
	virtual void reset() override;

private:
	CounterValue m_value;
	QString m_name;
	Common::CounterData::Type m_type;
};

}

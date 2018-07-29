#include "statistic_counter.h"
#include "seo_spider_service_api_export.h"
#include "command.h"

namespace SeoSpider
{

StatisticCounter::StatisticCounter(const QString& counterName, QObject* parent)
	: QObject(parent) 
	, m_value(0)
	, m_name(counterName)
	, m_type(Common::CounterData::Type::UseCounter)
{
}

StatisticCounter::~StatisticCounter()
{
	Common::SeoSpiderServiceApiLoader::serviceApi()->commitCounterData(m_name.toLatin1(), m_value, m_type);
}

const QString& StatisticCounter::name() const
{
	return m_name;
}

StatisticCounter::CounterValue StatisticCounter::value() const
{
	return m_value.load();
}

void StatisticCounter::increment()
{
	m_value.fetch_add(1, std::memory_order_relaxed);
}

void StatisticCounter::commit()
{
	Common::SeoSpiderServiceApiLoader::serviceApi()->commitCounterData(m_name.toLatin1(), m_value, m_type);
	reset();
}

void StatisticCounter::reset()
{
	m_value.store(0, std::memory_order_relaxed);
}

}

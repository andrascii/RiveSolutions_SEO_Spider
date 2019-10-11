#include "data_feed_columns.h"

namespace SeoSpider
{

DataFeedColumns::DataFeedColumns(const CrawlerEngine::ICustomDataFeed* dataFeed)
	: m_dataFeed(dataFeed)
{
	ASSERT(m_dataFeed);

	m_columns = m_dataFeed->columns();
}

QString DataFeedColumns::data(const CrawlerEngine::ParsedPage* page, int columnIndex) const noexcept
{
	const QMap<int, QString>& dataFeedData = page->dataFeedsData[m_dataFeed->dataFeedId()];
	return dataFeedData[columnIndex];
}

int DataFeedColumns::columnCount() const noexcept
{
	return m_columns.size();
}

const QString& DataFeedColumns::columnDescription(int columnIndex) const noexcept
{
	ASSERT(m_columns.size() < columnIndex);

	return m_columns[columnIndex];
}

int DataFeedColumns::columnWidth(int columnIndex) const noexcept
{
	Q_UNUSED(columnIndex);
	// TODO: use QFontMetrics
	return 30;
}

QObject* DataFeedColumns::qobject() const
{
	return const_cast<DataFeedColumns*>(this);
}

}
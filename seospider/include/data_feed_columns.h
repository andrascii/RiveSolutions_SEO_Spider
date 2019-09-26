#pragma once

#include "iflexible_columns.h"
#include "icustom_data_feed.h"

namespace SeoSpider
{

class DataFeedColumns : public QObject, public IFlexibleColumns
{
	Q_OBJECT

public:
	DataFeedColumns(const CrawlerEngine::ICustomDataFeed* dataFeed);

	virtual QString data(const CrawlerEngine::ParsedPage* page, int columnIndex) const noexcept override;
	virtual int columnCount() const noexcept override;
	virtual const QString& columnDescription(int columnIndex) const noexcept override;
	virtual int columnWidth(int columnIndex) const noexcept override;
	virtual QObject* qobject() const override;

signals:
	virtual void columnCountChanged(int newColumnCount) override;

private:
	const CrawlerEngine::ICustomDataFeed* m_dataFeed;
	QStringList m_columns;
};

}
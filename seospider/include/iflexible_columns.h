#pragma once

namespace CrawlerEngine
{

struct ParsedPage;

}

namespace SeoSpider
{

class IFlexibleColumns
{
public:
	virtual QString data(const CrawlerEngine::ParsedPage* page, int columnIndex) const noexcept = 0;
	virtual int columnCount() const noexcept = 0;
	virtual const QString& columnDescription(int columnIndex) const noexcept = 0;
	virtual int columnWidth(int columnIndex) const noexcept = 0;
	virtual QObject* qobject() const = 0;

	// signals
	virtual void columnCountChanged(int newColumnCount) = 0;
};

}
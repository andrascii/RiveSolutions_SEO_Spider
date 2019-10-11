#pragma once

#include "parsed_page_info.h"
#include "imenu_data_provider.h"
#include "row_resource_type.h"

namespace SeoSpider
{

class IStorageAdapter : public IMenuDataProvider
{
public:
	enum class ItemType
	{
		UrlItemType,
		PlainItemType
	};

	virtual ~IStorageAdapter() = default;

	virtual int columnWidth(int columnNumber) const noexcept = 0;
	virtual int columnCount() const noexcept = 0;
	virtual bool columnEnabled(int column) const noexcept = 0;
	virtual int itemCount() const noexcept = 0;
	virtual QVariant item(const QModelIndex& index) const noexcept = 0;
	virtual ItemType itemType(const QModelIndex& index) const noexcept = 0;
	virtual RowResourceType resourceType(int row) const noexcept = 0;
	virtual QString columnDescription(int columnIndex) const noexcept = 0;
	virtual ParsedPageInfoPtr parsedPageInfoPtr(const QModelIndex& index) const noexcept = 0;
	virtual const CrawlerEngine::ParsedPage* parsedPage(const QModelIndex& index) const noexcept = 0;
	virtual QObject* qobject() noexcept = 0;

	// signal
	virtual void parsedPageInfoAdded(int rowIndex) const = 0;
	virtual void parsedPageInfoRemoved(int rowIndex) const = 0;
	virtual void parsedPageInfoReplaced(int rowIndex) const = 0;
	virtual void repaintIndicesRange(std::pair<int, int> indicesRange) const = 0;
	virtual void repaintColumn(int rowIndex) const = 0;
	virtual void beginClearData() const = 0;
	virtual void endClearData() const = 0;

	// TODO: use beginInsertColumns/endInsertColumns!
	virtual void customColumnCountChanged() const = 0;
};

class IParsedPageStorageAdapter : public IStorageAdapter
{
public:
	virtual void setAvailableColumns(const QVector<ParsedPageInfo::Column>& availableColumns) noexcept = 0;
	virtual const QVector<ParsedPageInfo::Column>& availableColumns() const noexcept = 0;

	virtual void setCurrentColumns(const QVector<ParsedPageInfo::Column>& currentColumns) noexcept = 0;
	virtual const QVector<ParsedPageInfo::Column>& currentColumns() const noexcept = 0;
};

class IPageLinksStorageAdapter : public IStorageAdapter
{
public:
	virtual void setAvailableColumns(QVector<ParsedPageInfo::PageLinksColumn> availableColumns) noexcept = 0;
	virtual QVector<ParsedPageInfo::PageLinksColumn> availableColumns() const noexcept = 0;
};

}

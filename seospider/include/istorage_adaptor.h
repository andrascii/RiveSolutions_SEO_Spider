#pragma once

#include "parsed_page_info.h"

namespace SeoSpider
{

class IStorageAdaptor
{
public:
	enum class ItemType
	{
		UrlItemType,
		PlainItemType
	};

	virtual ~IStorageAdaptor() = default;

	virtual int columnWidth(int columnNumber) const noexcept = 0;

	virtual int columnCount() const noexcept = 0;
	virtual int itemCount() const noexcept = 0;

	virtual QVariant item(const QModelIndex& index) const noexcept = 0;
	virtual ItemType itemType(const QModelIndex& index) const noexcept = 0;

	virtual QString columnDescription(int columnIndex) const noexcept = 0;

	virtual ParsedPageInfoPtr parsedPageInfoPtr(const QModelIndex& index) const noexcept = 0;

	virtual QObject* qobject() noexcept = 0;

	// signal
	virtual void parsedPageInfoAdded(int rowIndex) const = 0;
};

class IParsedPageStorageAdaptor : public IStorageAdaptor
{
public:
	virtual void setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept = 0;

	virtual QList<ParsedPageInfo::Column> availableColumns() const noexcept = 0;
};

class IPageLinksStorageAdaptor : public IStorageAdaptor
{
public:
	virtual void setAvailableColumns(QList<ParsedPageInfo::PageLinksColumn> availableColumns) noexcept = 0;

	virtual QList<ParsedPageInfo::PageLinksColumn> availableColumns() const noexcept = 0;
};

}

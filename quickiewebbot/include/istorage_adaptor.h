#pragma once

#include "parsed_page_info.h"

namespace QuickieWebBot
{

class IStorageAdaptor
{
public:
	virtual void setAvailableColumns(QList<ParsedPageInfo::Column> availableColumns) noexcept = 0;
	virtual QList<ParsedPageInfo::Column> availableColumns() const noexcept = 0;
	virtual QString columnDescription(int columnIndex) const noexcept = 0;

	virtual int itemCount() const noexcept = 0;
	virtual QVariant item(const QModelIndex& index) const noexcept = 0;
	virtual ParsedPageInfo::Column itemType(const QModelIndex& index) const noexcept = 0;

	virtual PageRawInfoPtr pageRawInfoPtr(const QModelIndex& index) const noexcept = 0;

	virtual QObject* qobject() noexcept = 0;

	// signal
	virtual void pageRawInfoAdded(int rowIndex) const = 0;
};

}

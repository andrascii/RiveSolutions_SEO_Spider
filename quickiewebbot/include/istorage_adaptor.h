#pragma once

#include "page_raw_info.h"

namespace QuickieWebBot
{

class IStorageAdaptor
{
public:
	virtual void setAvailableColumns(QList<PageRawInfo::Column> availableColumns) noexcept = 0;
	virtual QList<PageRawInfo::Column> availableColumns() const noexcept = 0;
	virtual QString columnDescription(int columnIndex) const noexcept = 0;

	virtual int itemCount() const noexcept = 0;
	virtual QVariant item(const QModelIndex& index) const noexcept = 0;
	virtual PageRawInfo::Column itemType(const QModelIndex& index) const noexcept = 0;

	virtual PageRawInfoPtr pageRawInfoPtr(const QModelIndex& index) const noexcept = 0;

	virtual QObject* qobject() noexcept = 0;

	// signal
	virtual void pageRawInfoAdded(int rowIndex) const = 0;
};

}

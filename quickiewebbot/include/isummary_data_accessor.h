#pragma once

#include "summary_category_item.h"

namespace QuickieWebBot
{

class ISummaryDataAccessor
{
public:
	virtual SummaryCategoryItem itemCategory(const QModelIndex& index) const noexcept = 0;

	virtual Qt::ItemFlags flags(const QModelIndex& index) const noexcept = 0;

	virtual bool isHeaderItem(const QModelIndex& index) const noexcept = 0;

	virtual const QPixmap& pixmap(const QModelIndex& index) const noexcept = 0;

	virtual QVariant item(const QModelIndex& index) const noexcept = 0;

	virtual QSize span(const QModelIndex& index) const noexcept = 0;

	virtual int columnCount() const noexcept = 0;

	virtual int rowCount() const noexcept = 0;


	virtual QObject* qobject() noexcept = 0;

	// signals

	virtual void dataChanged(int row, int column, Qt::ItemDataRole role) const = 0;
};

}
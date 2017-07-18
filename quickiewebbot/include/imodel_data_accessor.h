#pragma once

#include "page_info.h"
#include "grid_view_painter.h"

namespace QuickieWebBot
{

class IModelDataAccessor
{
public:
	enum ItemFlag
	{
		ItemFlagNone = 0,
		ItemFlagUrl = 1,
		ItemFlagHasPixmap = 1 << 1,
		ItemFlagTextBold = 1 << 2,
		ItemFlagAlignRight = 1 << 3,
		ItemFlagAlignCenter = 1 << 4,
		ItemFlagNotSelectable = 1 << 5
	};

	virtual ~IModelDataAccessor() = default;
	
	virtual int columnCount() const = 0;
	virtual QString columnText(int column) const = 0;

	virtual int rowCount() const = 0;
	virtual QVariant itemValue(const QModelIndex& index) const = 0;
	virtual QColor itemBackgroundColor(const QModelIndex& index) const = 0;
	virtual QColor itemTextColor(const QModelIndex& index) const = 0;
	virtual int itemColSpan(const QModelIndex& index) const = 0;

	virtual QPixmap* pixmap(const QModelIndex& index) const = 0;

	virtual int flags(const QModelIndex& index) const = 0;

	virtual QObject* qobject() = 0;
	
	// signals
	virtual void itemChanged(QModelIndex index) = 0;
	virtual void rowRemoved(int row) = 0;
	virtual void rowAdded(int row) = 0;
	virtual void reset() = 0;

	virtual std::vector<GridViewPainter*> painters(const QModelIndex& index) const = 0;
};

class ModelDataAccessorBase : public IModelDataAccessor
{
public:
	virtual QColor itemTextColor(const QModelIndex& index) const override
	{
		// not working, why?
		return flags(index) & IModelDataAccessor::ItemFlagUrl ? QColor("#4753C5") : Qt::black;
	}
};

}
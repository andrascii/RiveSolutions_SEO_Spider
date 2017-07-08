#pragma once

#include "page_info.h"

namespace QuickieWebBot
{

class IModelDataAccessorItem
{
public:
	enum ItemFlag
	{
		ItemFlagNone = 0,
		ItemFlagUrl = 1
	};

	virtual ~IModelDataAccessorItem() = default;
	
	virtual int columnCount() const = 0;
	virtual QString columnText(int column) const = 0;

	virtual int rowCount() const = 0;
	virtual QVariant itemValue(const QModelIndex& index) const = 0;
	virtual QColor itemBackgroundColor(const QModelIndex& index) const = 0;
	virtual QColor itemTextColor(const QModelIndex& index) const = 0;
	virtual int itemColSpan(const QModelIndex& index) const = 0;

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const = 0;
	virtual QPixmap* pixmap(const QModelIndex& index) const = 0;

	virtual int flags(const QModelIndex& index) const = 0;

	virtual QObject* qobject() = 0;
	
	// signals
	virtual void itemChanged(QModelIndex index) = 0;
	virtual void rowRemoved(int row) = 0;
	virtual void rowAdded(int row) = 0;
	virtual void reset() = 0;
};

class ModelDataAccessorItemBase : public IModelDataAccessorItem
{
public:
	virtual QColor itemTextColor(const QModelIndex& index) const override
	{
		// not working, why?
		return flags(index) & IModelDataAccessorItem::ItemFlagUrl ? Qt::blue : Qt::black;
	}
};

class IModelDataAccessor
{
public:
	virtual ~IModelDataAccessor() = default;
	virtual std::unique_ptr<IModelDataAccessorItem> allProcessedItems() const = 0;
	// TODO: add more
};

}
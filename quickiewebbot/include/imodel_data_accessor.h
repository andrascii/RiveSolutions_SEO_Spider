#pragma once

#include "website_analyse_element.h"
#include <QAbstractItemDelegate>

namespace QuickieWebBot
{


class IModelDataAccessorItem
{

public:
	virtual ~IModelDataAccessorItem() = default;
	
	virtual int columnCount() const = 0;
	virtual QString columnText(int column) const = 0;

	virtual int rowCount() const = 0;
	virtual QVariant itemValue(const QModelIndex& index) const = 0;
	virtual int itemColSpan(const QModelIndex& index) const = 0;

	virtual QAbstractItemDelegate* itemDelegate(const QModelIndex& index) const = 0;

	virtual QObject* qobject() = 0;
	
	// signals
	virtual void itemChanged(QModelIndex index) = 0;
	virtual void rowRemoved(int row) = 0;
	virtual void rowAdded(int row) = 0;
	virtual void reset() = 0;
};

class IModelDataAccessor
{
public:
	virtual ~IModelDataAccessor() = default;
	virtual std::unique_ptr<IModelDataAccessorItem> allProcessedItems() const = 0;
	// TODO: add more
};



}
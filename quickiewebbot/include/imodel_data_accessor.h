#pragma once

#include "page_info.h"
#include "grid_view_painter.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class IGridViewResizeStrategy;

class IModelDataAccessor
{
public:
	virtual ~IModelDataAccessor() = default;

	virtual QList<PageInfo::ItemType> supportedColumns() const = 0;

	virtual int itemCount() const = 0;
	virtual QVariant item(const QModelIndex& index) const = 0;
	virtual QVariant item(int row, int column) const = 0;

	virtual PageInfoPtr pageInfoAtRow(int row) const = 0;

	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const = 0;
	virtual IGridViewResizeStrategy* resizeStrategy() const = 0;

	virtual QObject* qobject() = 0;

	//
	// signals
	//
	virtual void itemChanged(int row, int column) = 0;
	virtual void rowRemoved(int row) = 0;
	virtual void rowAdded(int row) = 0;
	virtual void reset() = 0;
};

}
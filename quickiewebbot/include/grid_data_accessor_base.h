#pragma once

#include "igrid_data_accessor.h"
#include "grid_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class GridDataAccessorBase : public IGridDataAccessor
{
public:

// 	QColor ModelDataAccessorBase::itemTextColor(const QModelIndex& index) const
// 	{
// 		return flags(index) & IModelDataAccessor::ItemFlagUrl ? QColor("#4753C5") : Qt::black;
// 	}

	virtual GridDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
};

}
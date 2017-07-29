#pragma once

#include "imodel_data_accessor.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class ModelDataAccessorBase : public IModelDataAccessor
{
public:

// 	QColor ModelDataAccessorBase::itemTextColor(const QModelIndex& index) const
// 	{
// 		return flags(index) & IModelDataAccessor::ItemFlagUrl ? QColor("#4753C5") : Qt::black;
// 	}

	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
};

}
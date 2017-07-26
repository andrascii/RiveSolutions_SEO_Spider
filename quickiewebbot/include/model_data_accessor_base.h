#pragma once

#include "imodel_data_accessor.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class ModelDataAccessorBase : public IModelDataAccessor
{
public:
	virtual QColor itemTextColor(const QModelIndex& index) const override;

	virtual ModelDataAccessorFactoryParams childViewParams(const QItemSelection& selection) const override;
};

}
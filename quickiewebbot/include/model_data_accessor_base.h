#pragma once
#include "imodel_data_accessor.h"

namespace QuickieWebBot
{

class ModelDataAccessorBase : public IModelDataAccessor
{
public:
	virtual QColor itemTextColor(const QModelIndex& index) const override;

	virtual std::unique_ptr<ModelDataAccessorFactoryParams> childViewParams(const QItemSelection& selection) const override;
};

}
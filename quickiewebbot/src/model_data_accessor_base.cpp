#include "model_data_accessor_base.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

QColor ModelDataAccessorBase::itemTextColor(const QModelIndex& index) const
{
	return flags(index) & IModelDataAccessor::ItemFlagUrl ? QColor("#4753C5") : Qt::black;
}

ModelDataAccessorFactoryParams ModelDataAccessorBase::childViewParams(const QItemSelection & selection) const
{
	return ModelDataAccessorFactoryParams(ModelDataAccessorFactoryParams::TypeInvalid, ModelDataAccessorFactoryParams::ModeGeneral);
}

}
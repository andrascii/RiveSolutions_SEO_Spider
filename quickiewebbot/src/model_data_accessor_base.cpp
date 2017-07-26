#include "model_data_accessor_base.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

QColor ModelDataAccessorBase::itemTextColor(const QModelIndex& index) const
{
	return flags(index) & IModelDataAccessor::ItemFlagUrl ? QColor("#4753C5") : Qt::black;
}

std::unique_ptr<ModelDataAccessorFactoryParams> ModelDataAccessorBase::childViewParams(const QItemSelection & selection) const
{
	using FP = ModelDataAccessorFactoryParams;
	return std::make_unique<FP>(FP::TypeInvalid, FP::ModeGeneral);
}

}
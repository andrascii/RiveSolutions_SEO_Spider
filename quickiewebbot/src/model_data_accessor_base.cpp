#include "model_data_accessor_base.h"
#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

ModelDataAccessorFactoryParams ModelDataAccessorBase::childViewParams(const QItemSelection & selection) const
{
	return ModelDataAccessorFactoryParams(ModelDataAccessorFactoryParams::TypeInvalid, ModelDataAccessorFactoryParams::ModeGeneral);
}

}
#include "grid_data_accessor_base.h"
#include "grid_data_accessor_factory_params.h"

namespace QuickieWebBot
{

GridDataAccessorFactoryParams GridDataAccessorBase::childViewParams(const QItemSelection & selection) const
{
	return GridDataAccessorFactoryParams(GridDataAccessorFactoryParams::TypeInvalid, GridDataAccessorFactoryParams::ModeGeneral);
}

}
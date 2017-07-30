#pragma once

#include "grid_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class IGridDataAccessor;

class GridDataAccessorFactory
{
public:
	std::unique_ptr<IGridDataAccessor> create(const GridDataAccessorFactoryParams& params);
};

}
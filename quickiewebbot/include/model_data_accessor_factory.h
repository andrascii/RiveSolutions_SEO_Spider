#pragma once

#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class IGridModelDataAccessor;

class ModelDataAccessorFactory
{
public:
	std::unique_ptr<IGridModelDataAccessor> create(const ModelDataAccessorFactoryParams& params);
};

}
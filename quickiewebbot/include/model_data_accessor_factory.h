#pragma once

#include "model_data_accessor_factory_params.h"

namespace QuickieWebBot
{

class IModelDataAccessor;

class ModelDataAccessorFactory
{
public:
	std::unique_ptr<IModelDataAccessor> create(const ModelDataAccessorFactoryParams& params);
};

}
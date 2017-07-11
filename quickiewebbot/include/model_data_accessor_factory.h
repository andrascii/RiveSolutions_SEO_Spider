#pragma once

namespace QuickieWebBot
{

class IModelDataAccessor;

struct ModelDataAccessorFactoryParams
{
	// TODO: add params

};

class ModelDataAccessorFactory
{

public:
	std::unique_ptr<IModelDataAccessor> getModelDataAccessor(const ModelDataAccessorFactoryParams& params);

};

}
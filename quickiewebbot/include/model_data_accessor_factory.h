#pragma once

namespace QuickieWebBot
{

class IModelDataAccessor;

struct ModelDataAccessorFactoryParams
{
	enum Type 
	{
		TypeAllCrawledUrls,
		TypeSummary
	};

	Type accessorType;

	// TODO: add params

};

class ModelDataAccessorFactory
{

public:
	std::unique_ptr<IModelDataAccessor> getModelDataAccessor(const ModelDataAccessorFactoryParams& params);

};

}
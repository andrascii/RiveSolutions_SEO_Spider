#include "model_data_accessor_factory.h"
#include "model_data_accessor_all_items.h"
#include "model_data_accessor_summary.h"

namespace QuickieWebBot
{


std::unique_ptr<IModelDataAccessor> ModelDataAccessorFactory::getModelDataAccessor(const ModelDataAccessorFactoryParams& params)
{
	Q_UNUSED(params);

	// TODO: return data accessor by the corresponding params

	if (params.accessorType == ModelDataAccessorFactoryParams::TypeAllCrawledUrls)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::CrawledUrlStorageType);
	}

	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummary)
	{
		return std::make_unique<ModelDataAccessorSummary>();
	}

	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesEmpty)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::EmptyTitleUrlStorageType);
	}

	assert(!"Invalid params");
	return std::unique_ptr<IModelDataAccessor>();
}


}
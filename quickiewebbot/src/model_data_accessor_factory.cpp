#include "model_data_accessor_factory.h"
#include "model_data_accessor.h"

namespace QuickieWebBot
{


	std::unique_ptr<IModelDataAccessor> ModelDataAccessorFactory::getModelDataAccessor(const ModelDataAccessorFactoryParams& params)
{
	Q_UNUSED(params);

	// TODO: return data accessor by the corresponding params

	return std::make_unique<ModelDataAccessorAllItems>(DataCollection::CrawledUrlStorageType);
}


}
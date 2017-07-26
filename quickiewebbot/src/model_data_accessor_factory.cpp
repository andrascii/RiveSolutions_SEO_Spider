#include "model_data_accessor_factory.h"
#include "model_data_accessor_all_items.h"
#include "model_data_accessor_one_item.h"
#include "model_data_accessor_summary.h"

namespace QuickieWebBot
{


std::unique_ptr<IModelDataAccessor> ModelDataAccessorFactory::getModelDataAccessor(const ModelDataAccessorFactoryParams& params)
{
	Q_UNUSED(params);

	//
	// TODO: we need to ability to change grid view column items for IModelDataAccessor
	// or add for each problem criteria one model data accessor
	// etc. ModelDataAccessorMetaDescriptions for all meta description problems
	//

	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummary)
	{
		return std::make_unique<ModelDataAccessorSummary>();
	}

	if (params.mode != ModelDataAccessorFactoryParams::ModeOneRow)
	{
		return std::make_unique<ModelDataAccessorAllItems>(static_cast<DataCollection::StorageType>(params.accessorType));
	}

	assert(params.row > -1);
	return std::make_unique<ModelDataAccessorOneItem>(static_cast<DataCollection::StorageType>(params.accessorType),  params.row);

	assert(!"Invalid params");
	return std::unique_ptr<IModelDataAccessor>();
}


}
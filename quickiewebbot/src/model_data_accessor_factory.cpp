#include "model_data_accessor_factory.h"
#include "model_data_accessor_all_items.h"
#include "model_data_accessor_one_item.h"
#include "model_data_accessor_summary.h"

namespace QuickieWebBot
{

std::unique_ptr<IGridModelDataAccessor> ModelDataAccessorFactory::create(const ModelDataAccessorFactoryParams& params)
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

	auto storageType = static_cast<DataCollection::StorageType>(params.accessorType);

	if (params.mode == ModelDataAccessorFactoryParams::ModeGeneral)
	{
		return std::make_unique<ModelDataAccessorAllItems>(storageType, QVector<PageInfo::ItemType>());
	}

	if (params.mode == ModelDataAccessorFactoryParams::ModeSummary)
	{
		QVector<PageInfo::ItemType> columns;

		switch (storageType)
		{
			//
			// Url problems
			//
			case DataCollection::UpperCaseUrlStorageType:
			case DataCollection::NonAsciiCharacterUrlStorageType:
			case DataCollection::VeryLongUrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::UrlLengthItemType, 
					PageInfo::ContentItemType, 
					PageInfo::StatusCodeItemType 
				};

				break;
			}
		
			case DataCollection::EmptyTitleUrlStorageType:
			case DataCollection::DuplicatedTitleUrlStorageType:
			case DataCollection::VeryLongTitleUrlStorageType:
			case DataCollection::VeryShortTitleUrlStorageType:
			case DataCollection::DuplicatedH1TitleUrlStorageType:
			case DataCollection::SeveralTitleUrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::ContentItemType, 
					PageInfo::TitleItemType, 
					PageInfo::TitleLengthItemType
				};
			
				break;
			}
			
			case DataCollection::EmptyMetaDescriptionUrlStorageType:
			case DataCollection::DuplicatedMetaDescriptionUrlStorageType:
			case DataCollection::VeryLongMetaDescriptionUrlStorageType:
			case DataCollection::VeryShortMetaDescriptionUrlStorageType:
			case DataCollection::SeveralMetaDescriptionUrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::ContentItemType, 
					PageInfo::MetaDescriptionItemType, 
					PageInfo::MetaDescriptionLengthItemType
				};
			
				break;
			}

			case DataCollection::EmptyMetaKeywordsUrlStorageType:
			case DataCollection::DuplicatedMetaKeywordsUrlStorageType:
			case DataCollection::SeveralMetaKeywordsUrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::ContentItemType, 
					PageInfo::MetaKeywordsItemType, 
					PageInfo::MetaKeywordsLengthItemType
				};
			
				break;
			}

			case DataCollection::MissingH1UrlStorageType:
			case DataCollection::DuplicatedH1UrlStorageType:
			case DataCollection::VeryLongH1UrlStorageType:
			case DataCollection::SeveralH1UrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::ContentItemType, 
					PageInfo::FirstH1ItemType, 
					PageInfo::FirstH1LengthItemType, 
					PageInfo::SecondH1ItemType, 
					PageInfo::SecondH1LengthItemType 
				};
			
				break;
			}

			case DataCollection::MissingH2UrlStorageType:
			case DataCollection::DuplicatedH2UrlStorageType:
			case DataCollection::VeryLongH2UrlStorageType:
			case DataCollection::SeveralH2UrlStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::ContentItemType, 
					PageInfo::FirstH2ItemType, 
					PageInfo::FirstH2LengthItemType, 
					PageInfo::SecondH2ItemType, 
					PageInfo::SecondH2LengthItemType 
				};
			
				break;
			}

			case DataCollection::Over100kbImageStorageType:
			case DataCollection::MissingAltTextImageStorageType:
			case DataCollection::VeryLongAltTextImageStorageType:
			{
				columns = 
				{ 
					PageInfo::UrlItemType, 
					PageInfo::AltTextItemType, 
					PageInfo::AltTextLengthItemType, 
					PageInfo::ImageSizeKbItemType
				};

				break;
			}
		
			default: assert(!"Unsupported params");
		}

		return std::make_unique<ModelDataAccessorAllItems>(storageType, columns);
	}

	assert(params.row > -1);
	//return std::make_unique<ModelDataAccessorOneItem>(storageType,  params.row);

	assert(!"Invalid params");
	return std::unique_ptr<IGridModelDataAccessor>();
}

}
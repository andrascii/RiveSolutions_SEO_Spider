#include "model_data_accessor_factory.h"
#include "model_data_accessor_all_items.h"
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

	//
	// Url problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryLinksNonAscii)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::NonAsciiCharacterUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryLinksUpperCase)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::UpperCaseUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryLinksTooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongUrlStorageType);
	}

	//
	// Title problems
	//
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
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesDuplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedTitleUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesTooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongTitleUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesTooShort)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryShortTitleUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesH1Duplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedH1TitleUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryTitlesSeveralTags)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::SeveralTitleUrlStorageType);
	}

	//
	// Meta description problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaDescriptionsEmpty)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::EmptyMetaDescriptionUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaDescriptionsDuplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedMetaDescriptionUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaDescriptionsTooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongMetaDescriptionUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaDescriptionsTooShort)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryShortMetaDescriptionUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaDescriptionsSeveralTags)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::SeveralMetaDescriptionUrlStorageType);
	}

	//
	// Meta keywords problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaKeyWordsDuplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedMetaKeywordsUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaKeyWordsEmpty)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::EmptyMetaKeywordsUrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryMetaKeyWordsSeveralTags)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::SeveralMetaKeywordsUrlStorageType);
	}

	//
	// H1 problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH1Missing)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::MissingH1UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH1Duplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedH1UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH1TooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongH1UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH1SeveralTags)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::SeveralH1UrlStorageType);
	}

	//
	// H2 problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH2Missing)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::MissingH2UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH2Duplicates)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::DuplicatedH2UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH2TooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongH2UrlStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryH2SeveralTags)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::SeveralH2UrlStorageType);
	}

	//
	// Images problems
	//
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryImagesOver100KB)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::Over100kbImageStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryImagesAltMissing)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::MissingAltTextImageStorageType);
	}
	if (params.accessorType == ModelDataAccessorFactoryParams::TypeSummaryImagesAltTooLong)
	{
		return std::make_unique<ModelDataAccessorAllItems>(DataCollection::VeryLongAltTextImageStorageType);
	}

	assert(!"Invalid params");
	return std::unique_ptr<IModelDataAccessor>();
}


}
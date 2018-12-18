#include "supported_resource_type_filtering_storages.h"

namespace
{
	std::vector<SeoSpider::StorageAdapterType> supportedFilteringStorages
	{
		SeoSpider::StorageAdapterType::StorageAdapterTypeAllPages,
		SeoSpider::StorageAdapterType::StorageAdapterTypeUpperCaseLinks,
		SeoSpider::StorageAdapterType::StorageAdapterTypeNonAsciiLinks,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooLongLinks,
		SeoSpider::StorageAdapterType::StorageAdapterTypeBrokenLinks,
		SeoSpider::StorageAdapterType::StorageAdapterTypeStatus4xx,
		SeoSpider::StorageAdapterType::StorageAdapterTypeStatus5xx,
		SeoSpider::StorageAdapterType::StorageAdapterTypeStatus302,
		SeoSpider::StorageAdapterType::StorageAdapterTypeStatus301,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooManyRedirections,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTimeout,

		// all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeEmptyTitles,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedTitles,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooLongTitles,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooShortTitles,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles,
		SeoSpider::StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage,*/

		// all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions,
		SeoSpider::StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage,*/

		// all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords,
		SeoSpider::StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage,*/

		// all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeMissingH1s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedH1s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooLongH1s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeSeveralH1s,*/

		// all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeMissingH2s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeDuplicatedH2s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeTooLongH2s,
		SeoSpider::StorageAdapterType::StorageAdapterTypeSeveralH2s,*/

		// images enum mappers - all images
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeImagesOver100kb,
		SeoSpider::StorageAdapterType::StorageAdapterTypeImageMissingAltText,
		SeoSpider::StorageAdapterType::StorageAdapterTypeImagesTooLongAltText,*/

		// page problems enum mappers - all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeTooManyLinksOnPage,
		SeoSpider::StorageAdapterType::StorageAdapterTypeContainsMetaRefreshTag,
		SeoSpider::StorageAdapterType::StorageAdapterTypeContainsFrames,*/

		// not indexed pages - all treaded as html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeBlockedForSEIndexing,
		SeoSpider::StorageAdapterType::StorageAdapterTypeNofollowLinks,
		SeoSpider::StorageAdapterType::StorageAdapterTypeBlockedByRobotsTxt,
		SeoSpider::StorageAdapterType::StorageAdapterTypeBlockedByXRobotsTag,*/

		// internal resources enum mappers - specific resource type
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeHtmlResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeImageResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeJavaScriptResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeStyleSheetResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeFlashResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeVideoResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeOtherResources,*/

		// external resources enum mappers - specific resource type
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeExternalHtmlResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalImageResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalJavaScriptResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalStyleSheetResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalFlashResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalVideoResources,
		SeoSpider::StorageAdapterType::StorageAdapterTypeExternalOtherResources,*/

		// yandex metrica - all html
		/*SeoSpider::StorageAdapterType::StorageAdapterTypeYandexMetricaCounter1,
		SeoSpider::StorageAdapterType::StorageAdapterTypeYandexMetricaCounter2,
		SeoSpider::StorageAdapterType::StorageAdapterTypeYandexMetricaCounter3,
		SeoSpider::StorageAdapterType::StorageAdapterTypeYandexMetricaCounter4,
		SeoSpider::StorageAdapterType::StorageAdapterTypeYandexMetricaCounter5,*/
	};
}

namespace SeoSpider
{

bool storageSupportsResourceTypeFiltering(StorageAdapterType storageType)
{
	const auto it = std::find(supportedFilteringStorages.cbegin(), supportedFilteringStorages.cend(), storageType);
	return it != supportedFilteringStorages.cend();
}

}

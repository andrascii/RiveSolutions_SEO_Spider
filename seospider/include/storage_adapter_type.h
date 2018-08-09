#pragma once

#include "unordered_data_collection.h"

namespace SeoSpider
{

enum class StorageAdapterType
{
	StorageAdapterTypeBegin,
	StorageAdapterTypeNone,

	StorageAdapterTypeAllPages = CrawlerEngine::StorageType::CrawledUrlStorageType,
	StorageAdapterTypeUpperCaseLinks = CrawlerEngine::StorageType::UpperCaseUrlStorageType,
	StorageAdapterTypeNonAsciiLinks = CrawlerEngine::StorageType::NonAsciiCharacterUrlStorageType,
	StorageAdapterTypeTooLongLinks = CrawlerEngine::StorageType::TooLongUrlStorageType,
	StorageAdapterTypeBrokenLinks = CrawlerEngine::StorageType::BrokenLinks,
	StorageAdapterTypeStatus4xx = CrawlerEngine::StorageType::Status4xxStorageType,
	StorageAdapterTypeStatus5xx = CrawlerEngine::StorageType::Status5xxStorageType,
	StorageAdapterTypeStatus302 = CrawlerEngine::StorageType::Status302StorageType,
	StorageAdapterTypeStatus301 = CrawlerEngine::StorageType::Status301StorageType,
	StorageAdapterTypeTooManyRedirections = CrawlerEngine::StorageType::TooManyRedirectsStorageType,
	StorageAdapterTypeTimeout = CrawlerEngine::StorageType::TimeoutStorageType,

	StorageAdapterTypeEmptyTitles = CrawlerEngine::StorageType::EmptyTitleUrlStorageType,
	StorageAdapterTypeDuplicatedTitles = CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType,
	StorageAdapterTypeTooLongTitles = CrawlerEngine::StorageType::TooLongTitleUrlStorageType,
	StorageAdapterTypeTooShortTitles = CrawlerEngine::StorageType::TooShortTitleUrlStorageType,
	StorageAdapterTypeDuplicatedH1Titles = CrawlerEngine::StorageType::DuplicatedH1TitleUrlStorageType,
	StorageAdapterTypeSeveralTitlesOnPage = CrawlerEngine::StorageType::SeveralTitleUrlStorageType,

	StorageAdapterTypeEmptyMetaDescriptions = CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType,
	StorageAdapterTypeDuplicatedMetaDescriptions = CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType,
	StorageAdapterTypeTooLongMetaDescriptions = CrawlerEngine::StorageType::TooLongMetaDescriptionUrlStorageType,
	StorageAdapterTypeTooShortMetaDescriptions = CrawlerEngine::StorageType::TooShortMetaDescriptionUrlStorageType,
	StorageAdapterTypeSeveralMetaDescriptionsOnPage = CrawlerEngine::StorageType::SeveralMetaDescriptionUrlStorageType,

	StorageAdapterTypeEmptyMetaKeywords = CrawlerEngine::StorageType::EmptyMetaKeywordsUrlStorageType,
	StorageAdapterTypeDuplicatedMetaKeywords = CrawlerEngine::StorageType::DuplicatedMetaKeywordsUrlStorageType,
	StorageAdapterTypeSeveralMetaKeywordsOnPage = CrawlerEngine::StorageType::SeveralMetaKeywordsUrlStorageType,

	StorageAdapterTypeMissingH1s = CrawlerEngine::StorageType::MissingH1UrlStorageType,
	StorageAdapterTypeDuplicatedH1s = CrawlerEngine::StorageType::DuplicatedH1UrlStorageType,
	StorageAdapterTypeTooLongH1s = CrawlerEngine::StorageType::TooLongH1UrlStorageType,
	StorageAdapterTypeSeveralH1s = CrawlerEngine::StorageType::SeveralH1UrlStorageType,

	StorageAdapterTypeMissingH2s = CrawlerEngine::StorageType::MissingH2UrlStorageType,
	StorageAdapterTypeDuplicatedH2s = CrawlerEngine::StorageType::DuplicatedH2UrlStorageType,
	StorageAdapterTypeTooLongH2s = CrawlerEngine::StorageType::TooLongH2UrlStorageType,
	StorageAdapterTypeSeveralH2s = CrawlerEngine::StorageType::SeveralH2UrlStorageType,

	// images enum mappers
	StorageAdapterTypeImagesOver100kb = CrawlerEngine::StorageType::TooBigImageStorageType,
	StorageAdapterTypeImageMissingAltText = CrawlerEngine::StorageType::MissingAltTextImageStorageType,
	StorageAdapterTypeImagesTooLongAltText = CrawlerEngine::StorageType::TooLongAltTextImageStorageType,

	// page problems enum mappers
	StorageAdapterTypeTooManyLinksOnPage = CrawlerEngine::StorageType::TooManyLinksOnPageStorageType,
	StorageAdapterTypeContainsMetaRefreshTag = CrawlerEngine::StorageType::ContainsMetaRefreshTagStorageType,
	StorageAdapterTypeContainsFrames = CrawlerEngine::StorageType::ContainsFramesStorageType,

	// not indexed pages
	StorageAdapterTypeBlockedForSEIndexing = CrawlerEngine::StorageType::BlockedForSEIndexingStorageType,
	StorageAdapterTypeNofollowLinks = CrawlerEngine::StorageType::NofollowLinksStorageType,
	StorageAdapterTypeBlockedByRobotsTxt = CrawlerEngine::StorageType::BlockedByRobotsTxtStorageType,
	StorageAdapterTypeBlockedByXRobotsTag = CrawlerEngine::StorageType::BlockedByXRobotsTagStorageType,

	// internal resources enum mappers
	StorageAdapterTypeHtmlResources = CrawlerEngine::StorageType::HtmlResourcesStorageType,
	StorageAdapterTypeImageResources = CrawlerEngine::StorageType::ImageResourcesStorageType,
	StorageAdapterTypeJavaScriptResources = CrawlerEngine::StorageType::JavaScriptResourcesStorageType,
	StorageAdapterTypeStyleSheetResources = CrawlerEngine::StorageType::StyleSheetResourcesStorageType,
	StorageAdapterTypeFlashResources = CrawlerEngine::StorageType::FlashResourcesStorageType,
	StorageAdapterTypeVideoResources = CrawlerEngine::StorageType::VideoResourcesStorageType,
	StorageAdapterTypeOtherResources = CrawlerEngine::StorageType::OtherResourcesStorageType,

	// external resources enum mappers
	StorageAdapterTypeExternalHtmlResources = CrawlerEngine::StorageType::ExternalHtmlResourcesStorageType,
	StorageAdapterTypeExternalImageResources = CrawlerEngine::StorageType::ExternalImageResourcesStorageType,
	StorageAdapterTypeExternalJavaScriptResources = CrawlerEngine::StorageType::ExternalJavaScriptResourcesStorageType,
	StorageAdapterTypeExternalStyleSheetResources = CrawlerEngine::StorageType::ExternalStyleSheetResourcesStorageType,
	StorageAdapterTypeExternalFlashResources = CrawlerEngine::StorageType::ExternalFlashResourcesStorageType,
	StorageAdapterTypeExternalVideoResources = CrawlerEngine::StorageType::ExternalVideoResourcesStorageType,
	StorageAdapterTypeExternalOtherResources = CrawlerEngine::StorageType::ExternalOtherResourcesStorageType,

	StorageAdapterTypeNotFound, // not an actual storage but used for show QLabel that says about "has no search results"
	StorageAdapterTypeEnd
};

inline QString storageAdapterTypeStringFromEnum(StorageAdapterType type)
{
	switch(type)
	{
		case StorageAdapterType::StorageAdapterTypeAllPages: return "AllPages";
		case StorageAdapterType::StorageAdapterTypeUpperCaseLinks: return "UpperCaseLinks";
		case StorageAdapterType::StorageAdapterTypeNonAsciiLinks: return "NonAsciiLinks";
		case StorageAdapterType::StorageAdapterTypeTooLongLinks: return "TooLongLinks";
		case StorageAdapterType::StorageAdapterTypeBrokenLinks: return "BrokenLinks";
		case StorageAdapterType::StorageAdapterTypeStatus4xx: return "Status4xx";
		case StorageAdapterType::StorageAdapterTypeStatus5xx: return "Status5xx";
		case StorageAdapterType::StorageAdapterTypeStatus302: return "Status302";
		case StorageAdapterType::StorageAdapterTypeStatus301: return "Status301";
		case StorageAdapterType::StorageAdapterTypeTooManyRedirections: return "TooManyRedirections";
		case StorageAdapterType::StorageAdapterTypeTimeout: return "Timeout";

		case StorageAdapterType::StorageAdapterTypeEmptyTitles: return "EmptyTitles";
		case StorageAdapterType::StorageAdapterTypeDuplicatedTitles: return "DuplicatedTitles";
		case StorageAdapterType::StorageAdapterTypeTooLongTitles: return "TooLongTitles";
		case StorageAdapterType::StorageAdapterTypeTooShortTitles: return "TooShortTitles";
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1Titles: return "DuplicatedH1Titles";
		case StorageAdapterType::StorageAdapterTypeSeveralTitlesOnPage: return "SeveralTitlesOnPage";

		case StorageAdapterType::StorageAdapterTypeEmptyMetaDescriptions: return "EmptyMetaDescriptions";
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaDescriptions: return "DuplicatedMetaDescriptions";
		case StorageAdapterType::StorageAdapterTypeTooLongMetaDescriptions: return "TooLongMetaDescriptions";
		case StorageAdapterType::StorageAdapterTypeTooShortMetaDescriptions: return "TooShortMetaDescriptions";
		case StorageAdapterType::StorageAdapterTypeSeveralMetaDescriptionsOnPage: return "SeveralMetaDescriptionsOnPage";

		case StorageAdapterType::StorageAdapterTypeEmptyMetaKeywords: return "EmptyMetaKeywords";
		case StorageAdapterType::StorageAdapterTypeDuplicatedMetaKeywords: return "DuplicatedMetaKeywords";
		case StorageAdapterType::StorageAdapterTypeSeveralMetaKeywordsOnPage: return "SeveralMetaKeywordsOnPage";

		case StorageAdapterType::StorageAdapterTypeMissingH1s: return "MissingH1s";
		case StorageAdapterType::StorageAdapterTypeDuplicatedH1s: return "DuplicatedH1s";
		case StorageAdapterType::StorageAdapterTypeTooLongH1s: return "TooLongH1s";
		case StorageAdapterType::StorageAdapterTypeSeveralH1s: return "SeveralH1s";

		case StorageAdapterType::StorageAdapterTypeMissingH2s: return "MissingH2s";
		case StorageAdapterType::StorageAdapterTypeDuplicatedH2s: return "DuplicatedH2s";
		case StorageAdapterType::StorageAdapterTypeTooLongH2s: return "TooLongH2s";
		case StorageAdapterType::StorageAdapterTypeSeveralH2s: return "SeveralH2s";

		// images enum mappers
		case StorageAdapterType::StorageAdapterTypeImagesOver100kb: return "ImagesOver100kb";
		case StorageAdapterType::StorageAdapterTypeImageMissingAltText: return "ImageMissingAltText";
		case StorageAdapterType::StorageAdapterTypeImagesTooLongAltText: return "ImagesTooLongAltText";

		// page problems enum mappers
		case StorageAdapterType::StorageAdapterTypeTooManyLinksOnPage: return "TooManyLinksOnPage";
		case StorageAdapterType::StorageAdapterTypeContainsMetaRefreshTag: return "ContainsMetaRefreshTag";
		case StorageAdapterType::StorageAdapterTypeContainsFrames: return "ContainsFrames";

		// not indexed pages
		case StorageAdapterType::StorageAdapterTypeBlockedForSEIndexing: return "BlockedForSEIndexing";
		case StorageAdapterType::StorageAdapterTypeNofollowLinks: return "NofollowLinks";
		case StorageAdapterType::StorageAdapterTypeBlockedByRobotsTxt: return "BlockedByRobotsTxt";
		case StorageAdapterType::StorageAdapterTypeBlockedByXRobotsTag: return "BlockedByXRobotsTag";

		// internal resources enum mappers
		case StorageAdapterType::StorageAdapterTypeHtmlResources: return "HtmlResources";
		case StorageAdapterType::StorageAdapterTypeImageResources: return "ImageResources";
		case StorageAdapterType::StorageAdapterTypeJavaScriptResources: return "JavaScriptResources";
		case StorageAdapterType::StorageAdapterTypeStyleSheetResources: return "StyleSheetResources";
		case StorageAdapterType::StorageAdapterTypeFlashResources: return "FlashResources";
		case StorageAdapterType::StorageAdapterTypeVideoResources: return "VideoResources";
		case StorageAdapterType::StorageAdapterTypeOtherResources: return "OtherResources";

		// external resources enum mappers
		case StorageAdapterType::StorageAdapterTypeExternalHtmlResources: return "ExternalHtmlResources";
		case StorageAdapterType::StorageAdapterTypeExternalImageResources: return "ExternalImageResources";
		case StorageAdapterType::StorageAdapterTypeExternalJavaScriptResources: return "ExternalJavaScriptResources";
		case StorageAdapterType::StorageAdapterTypeExternalStyleSheetResources: return "ExternalStyleSheetResources";
		case StorageAdapterType::StorageAdapterTypeExternalFlashResources: return "ExternalFlashResources";
		case StorageAdapterType::StorageAdapterTypeExternalVideoResources: return "ExternalVideoResources";
		case StorageAdapterType::StorageAdapterTypeExternalOtherResources: return "ExternalOtherResources";

		default: ASSERT(false && "Incorrect storage type"); return QString();
	}
}

}
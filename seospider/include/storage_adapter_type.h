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

}
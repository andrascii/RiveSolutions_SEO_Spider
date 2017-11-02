#pragma once

#include "unordered_data_collection.h"

namespace SeoSpider
{

enum class StorageAdaptorType
{
	StorageAdaptorTypeBegin = CrawlerEngine::StorageType::BeginEnumStorageType,

	StorageAdaptorTypeNone,

	StorageAdaptorTypeAllPages = CrawlerEngine::StorageType::CrawledUrlStorageType,

	StorageAdaptorTypeUpperCaseLinks = CrawlerEngine::StorageType::UpperCaseUrlStorageType,
	StorageAdaptorTypeNonAsciiLinks = CrawlerEngine::StorageType::NonAsciiCharacterUrlStorageType,
	StorageAdaptorTypeVeryLongLinks = CrawlerEngine::StorageType::VeryLongUrlStorageType,
	StorageAdaptorTypeBrokenLinks = CrawlerEngine::StorageType::Status404StorageType,

	StorageAdaptorTypeEmptyTitles = CrawlerEngine::StorageType::EmptyTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedTitles = CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType,
	StorageAdaptorTypeVeryLongTitles = CrawlerEngine::StorageType::VeryLongTitleUrlStorageType,
	StorageAdaptorTypeVeryShortTitles = CrawlerEngine::StorageType::VeryShortTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedH1Titles = CrawlerEngine::StorageType::DuplicatedH1TitleUrlStorageType,
	StorageAdaptorTypeSeveralTitlesOnPage = CrawlerEngine::StorageType::SeveralTitleUrlStorageType,

	StorageAdaptorTypeEmptyMetaDescriptions = CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaDescriptions = CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryLongMetaDescriptions = CrawlerEngine::StorageType::VeryLongMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryShortMetaDescriptions = CrawlerEngine::StorageType::VeryShortMetaDescriptionUrlStorageType,
	StorageAdaptorTypeSeveralMetaDescriptionsOnPage = CrawlerEngine::StorageType::SeveralMetaDescriptionUrlStorageType,

	StorageAdaptorTypeEmptyMetaKeywords = CrawlerEngine::StorageType::EmptyMetaKeywordsUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaKeywords = CrawlerEngine::StorageType::DuplicatedMetaKeywordsUrlStorageType,
	StorageAdaptorTypeSeveralMetaKeywordsOnPage = CrawlerEngine::StorageType::SeveralMetaKeywordsUrlStorageType,

	StorageAdaptorTypeMissingH1s = CrawlerEngine::StorageType::MissingH1UrlStorageType,
	StorageAdaptorTypeDuplicatedH1s = CrawlerEngine::StorageType::DuplicatedH1UrlStorageType,
	StorageAdaptorTypeVeryLongH1s = CrawlerEngine::StorageType::VeryLongH1UrlStorageType,
	StorageAdaptorTypeSeveralH1s = CrawlerEngine::StorageType::SeveralH1UrlStorageType,

	StorageAdaptorTypeMissingH2s = CrawlerEngine::StorageType::MissingH2UrlStorageType,
	StorageAdaptorTypeDuplicatedH2s = CrawlerEngine::StorageType::DuplicatedH2UrlStorageType,
	StorageAdaptorTypeVeryLongH2s = CrawlerEngine::StorageType::VeryLongH2UrlStorageType,
	StorageAdaptorTypeSeveralH2s = CrawlerEngine::StorageType::SeveralH2UrlStorageType,

	StorageAdaptorTypeImagesOver100kb = CrawlerEngine::StorageType::Over100kbImageStorageType,
	StorageAdaptorTypeImageMissingAltText = CrawlerEngine::StorageType::MissingAltTextImageStorageType,
	StorageAdaptorTypeImagesVeryLongAltText = CrawlerEngine::StorageType::VeryLongAltTextImageStorageType,

	// internal resources enum mappers
	StorageAdaptorTypeHtmlResources = CrawlerEngine::StorageType::HtmlResourcesStorageType,
	StorageAdaptorTypeImageResources = CrawlerEngine::StorageType::ImageResourcesStorageType,
	StorageAdaptorTypeJavaScriptResources = CrawlerEngine::StorageType::JavaScriptResourcesStorageType,
	StorageAdaptorTypeStyleSheetResources = CrawlerEngine::StorageType::StyleSheetResourcesStorageType,
	StorageAdaptorTypeFlashResources = CrawlerEngine::StorageType::FlashResourcesStorageType,
	StorageAdaptorTypeVideoResources = CrawlerEngine::StorageType::VideoResourcesStorageType,
	StorageAdaptorTypeOtherResources = CrawlerEngine::StorageType::OtherResourcesStorageType,

	// external resources enum mappers
	StorageAdaptorTypeExternalHtmlResources = CrawlerEngine::StorageType::ExternalHtmlResourcesStorageType,
	StorageAdaptorTypeExternalImageResources = CrawlerEngine::StorageType::ExternalImageResourcesStorageType,
	StorageAdaptorTypeExternalJavaScriptResources = CrawlerEngine::StorageType::ExternalJavaScriptResourcesStorageType,
	StorageAdaptorTypeExternalStyleSheetResources = CrawlerEngine::StorageType::ExternalStyleSheetResourcesStorageType,
	StorageAdaptorTypeExternalFlashResources = CrawlerEngine::StorageType::ExternalFlashResourcesStorageType,
	StorageAdaptorTypeExternalVideoResources = CrawlerEngine::StorageType::ExternalVideoResourcesStorageType,
	StorageAdaptorTypeExternalOtherResources = CrawlerEngine::StorageType::ExternalOtherResourcesStorageType,

	StorageAdaptorTypeEnd = CrawlerEngine::StorageType::EndEnumStorageType
};

}
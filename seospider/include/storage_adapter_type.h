#pragma once

#include "unordered_data_collection.h"

namespace SeoSpider
{

enum class StorageAdapterType
{
	StorageAdapterTypeBegin = CrawlerEngine::StorageType::BeginEnumStorageType,

	StorageAdapterTypeNone,

	StorageAdapterTypeAllPages = CrawlerEngine::StorageType::CrawledUrlStorageType,

	StorageAdapterTypeUpperCaseLinks = CrawlerEngine::StorageType::UpperCaseUrlStorageType,
	StorageAdapterTypeNonAsciiLinks = CrawlerEngine::StorageType::NonAsciiCharacterUrlStorageType,
	StorageAdapterTypeVeryLongLinks = CrawlerEngine::StorageType::VeryLongUrlStorageType,
	StorageAdapterTypeBrokenLinks = CrawlerEngine::StorageType::BrokenLinks,
	StorageAdapterTypeStatus4xx = CrawlerEngine::StorageType::Status4xxStorageType,
	StorageAdapterTypeStatus5xx = CrawlerEngine::StorageType::Status5xxStorageType,
	StorageAdapterTypeStatus302 = CrawlerEngine::StorageType::Status302StorageType,
	StorageAdapterTypeStatus301 = CrawlerEngine::StorageType::Status301StorageType,

	StorageAdapterTypeEmptyTitles = CrawlerEngine::StorageType::EmptyTitleUrlStorageType,
	StorageAdapterTypeDuplicatedTitles = CrawlerEngine::StorageType::DuplicatedTitleUrlStorageType,
	StorageAdapterTypeVeryLongTitles = CrawlerEngine::StorageType::VeryLongTitleUrlStorageType,
	StorageAdapterTypeVeryShortTitles = CrawlerEngine::StorageType::VeryShortTitleUrlStorageType,
	StorageAdapterTypeDuplicatedH1Titles = CrawlerEngine::StorageType::DuplicatedH1TitleUrlStorageType,
	StorageAdapterTypeSeveralTitlesOnPage = CrawlerEngine::StorageType::SeveralTitleUrlStorageType,

	StorageAdapterTypeEmptyMetaDescriptions = CrawlerEngine::StorageType::EmptyMetaDescriptionUrlStorageType,
	StorageAdapterTypeDuplicatedMetaDescriptions = CrawlerEngine::StorageType::DuplicatedMetaDescriptionUrlStorageType,
	StorageAdapterTypeVeryLongMetaDescriptions = CrawlerEngine::StorageType::VeryLongMetaDescriptionUrlStorageType,
	StorageAdapterTypeVeryShortMetaDescriptions = CrawlerEngine::StorageType::VeryShortMetaDescriptionUrlStorageType,
	StorageAdapterTypeSeveralMetaDescriptionsOnPage = CrawlerEngine::StorageType::SeveralMetaDescriptionUrlStorageType,

	StorageAdapterTypeEmptyMetaKeywords = CrawlerEngine::StorageType::EmptyMetaKeywordsUrlStorageType,
	StorageAdapterTypeDuplicatedMetaKeywords = CrawlerEngine::StorageType::DuplicatedMetaKeywordsUrlStorageType,
	StorageAdapterTypeSeveralMetaKeywordsOnPage = CrawlerEngine::StorageType::SeveralMetaKeywordsUrlStorageType,

	StorageAdapterTypeMissingH1s = CrawlerEngine::StorageType::MissingH1UrlStorageType,
	StorageAdapterTypeDuplicatedH1s = CrawlerEngine::StorageType::DuplicatedH1UrlStorageType,
	StorageAdapterTypeVeryLongH1s = CrawlerEngine::StorageType::VeryLongH1UrlStorageType,
	StorageAdapterTypeSeveralH1s = CrawlerEngine::StorageType::SeveralH1UrlStorageType,

	StorageAdapterTypeMissingH2s = CrawlerEngine::StorageType::MissingH2UrlStorageType,
	StorageAdapterTypeDuplicatedH2s = CrawlerEngine::StorageType::DuplicatedH2UrlStorageType,
	StorageAdapterTypeVeryLongH2s = CrawlerEngine::StorageType::VeryLongH2UrlStorageType,
	StorageAdapterTypeSeveralH2s = CrawlerEngine::StorageType::SeveralH2UrlStorageType,

	StorageAdapterTypeImagesOver100kb = CrawlerEngine::StorageType::Over100kbImageStorageType,
	StorageAdapterTypeImageMissingAltText = CrawlerEngine::StorageType::MissingAltTextImageStorageType,
	StorageAdapterTypeImagesVeryLongAltText = CrawlerEngine::StorageType::VeryLongAltTextImageStorageType,

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

	StorageAdapterTypeEnd = CrawlerEngine::StorageType::EndEnumStorageType
};

}
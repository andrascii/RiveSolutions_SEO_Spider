#pragma once

#include "unordered_data_collection.h"

namespace SeoSpider
{

enum class StorageAdaptorType
{
	StorageAdaptorTypeBegin = WebCrawler::StorageType::BeginEnumStorageType,

	StorageAdaptorTypeNone,

	StorageAdaptorTypeAllPages = WebCrawler::StorageType::CrawledUrlStorageType,

	StorageAdaptorTypeUpperCaseLinks = WebCrawler::StorageType::UpperCaseUrlStorageType,
	StorageAdaptorTypeNonAsciiLinks = WebCrawler::StorageType::NonAsciiCharacterUrlStorageType,
	StorageAdaptorTypeVeryLongLinks = WebCrawler::StorageType::VeryLongUrlStorageType,
	StorageAdaptorTypeBrokenLinks = WebCrawler::StorageType::Status404StorageType,

	StorageAdaptorTypeEmptyTitles = WebCrawler::StorageType::EmptyTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedTitles = WebCrawler::StorageType::DuplicatedTitleUrlStorageType,
	StorageAdaptorTypeVeryLongTitles = WebCrawler::StorageType::VeryLongTitleUrlStorageType,
	StorageAdaptorTypeVeryShortTitles = WebCrawler::StorageType::VeryShortTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedH1Titles = WebCrawler::StorageType::DuplicatedH1TitleUrlStorageType,
	StorageAdaptorTypeSeveralTitlesOnPage = WebCrawler::StorageType::SeveralTitleUrlStorageType,

	StorageAdaptorTypeEmptyMetaDescriptions = WebCrawler::StorageType::EmptyMetaDescriptionUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaDescriptions = WebCrawler::StorageType::DuplicatedMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryLongMetaDescriptions = WebCrawler::StorageType::VeryLongMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryShortMetaDescriptions = WebCrawler::StorageType::VeryShortMetaDescriptionUrlStorageType,
	StorageAdaptorTypeSeveralMetaDescriptionsOnPage = WebCrawler::StorageType::SeveralMetaDescriptionUrlStorageType,

	StorageAdaptorTypeEmptyMetaKeywords = WebCrawler::StorageType::EmptyMetaKeywordsUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaKeywords = WebCrawler::StorageType::DuplicatedMetaKeywordsUrlStorageType,
	StorageAdaptorTypeSeveralMetaKeywordsOnPage = WebCrawler::StorageType::SeveralMetaKeywordsUrlStorageType,

	StorageAdaptorTypeMissingH1s = WebCrawler::StorageType::MissingH1UrlStorageType,
	StorageAdaptorTypeDuplicatedH1s = WebCrawler::StorageType::DuplicatedH1UrlStorageType,
	StorageAdaptorTypeVeryLongH1s = WebCrawler::StorageType::VeryLongH1UrlStorageType,
	StorageAdaptorTypeSeveralH1s = WebCrawler::StorageType::SeveralH1UrlStorageType,

	StorageAdaptorTypeMissingH2s = WebCrawler::StorageType::MissingH2UrlStorageType,
	StorageAdaptorTypeDuplicatedH2s = WebCrawler::StorageType::DuplicatedH2UrlStorageType,
	StorageAdaptorTypeVeryLongH2s = WebCrawler::StorageType::VeryLongH2UrlStorageType,
	StorageAdaptorTypeSeveralH2s = WebCrawler::StorageType::SeveralH2UrlStorageType,

	StorageAdaptorTypeImagesOver100kb = WebCrawler::StorageType::Over100kbImageStorageType,
	StorageAdaptorTypeImageMissingAltText = WebCrawler::StorageType::MissingAltTextImageStorageType,
	StorageAdaptorTypeImagesVeryLongAltText = WebCrawler::StorageType::VeryLongAltTextImageStorageType,

	// internal resources enum mappers
	StorageAdaptorTypeHtmlResources = WebCrawler::StorageType::HtmlResourcesStorageType,
	StorageAdaptorTypeImageResources = WebCrawler::StorageType::ImageResourcesStorageType,
	StorageAdaptorTypeJavaScriptResources = WebCrawler::StorageType::JavaScriptResourcesStorageType,
	StorageAdaptorTypeStyleSheetResources = WebCrawler::StorageType::StyleSheetResourcesStorageType,
	StorageAdaptorTypeFlashResources = WebCrawler::StorageType::FlashResourcesStorageType,
	StorageAdaptorTypeVideoResources = WebCrawler::StorageType::VideoResourcesStorageType,
	StorageAdaptorTypeOtherResources = WebCrawler::StorageType::OtherResourcesStorageType,

	// external resources enum mappers
	StorageAdaptorTypeExternalHtmlResources = WebCrawler::StorageType::ExternalHtmlResourcesStorageType,
	StorageAdaptorTypeExternalImageResources = WebCrawler::StorageType::ExternalImageResourcesStorageType,
	StorageAdaptorTypeExternalJavaScriptResources = WebCrawler::StorageType::ExternalJavaScriptResourcesStorageType,
	StorageAdaptorTypeExternalStyleSheetResources = WebCrawler::StorageType::ExternalStyleSheetResourcesStorageType,
	StorageAdaptorTypeExternalFlashResources = WebCrawler::StorageType::ExternalFlashResourcesStorageType,
	StorageAdaptorTypeExternalVideoResources = WebCrawler::StorageType::ExternalVideoResourcesStorageType,
	StorageAdaptorTypeExternalOtherResources = WebCrawler::StorageType::ExternalOtherResourcesStorageType,

	StorageAdaptorTypeEnd = WebCrawler::StorageType::EndEnumStorageType
};

}
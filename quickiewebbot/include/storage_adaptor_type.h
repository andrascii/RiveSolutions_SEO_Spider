#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

enum class StorageAdaptorType
{
	StorageAdaptorTypeBegin = WebCrawler::DataCollection::BeginEnumStorageType,

	StorageAdaptorTypeNone,

	StorageAdaptorTypeAllPages = WebCrawler::DataCollection::CrawledUrlStorageType,

	StorageAdaptorTypeUpperCaseLinks = WebCrawler::DataCollection::UpperCaseUrlStorageType,
	StorageAdaptorTypeNonAsciiLinks = WebCrawler::DataCollection::NonAsciiCharacterUrlStorageType,
	StorageAdaptorTypeVeryLongLinks = WebCrawler::DataCollection::VeryLongUrlStorageType,
	StorageAdaptorTypeBrokenLinks = WebCrawler::DataCollection::Status404StorageType,

	StorageAdaptorTypeEmptyTitles = WebCrawler::DataCollection::EmptyTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedTitles = WebCrawler::DataCollection::DuplicatedTitleUrlStorageType,
	StorageAdaptorTypeVeryLongTitles = WebCrawler::DataCollection::VeryLongTitleUrlStorageType,
	StorageAdaptorTypeVeryShortTitles = WebCrawler::DataCollection::VeryShortTitleUrlStorageType,
	StorageAdaptorTypeDuplicatedH1Titles = WebCrawler::DataCollection::DuplicatedH1TitleUrlStorageType,
	StorageAdaptorTypeSeveralTitlesOnPage = WebCrawler::DataCollection::SeveralTitleUrlStorageType,

	StorageAdaptorTypeEmptyMetaDescriptions = WebCrawler::DataCollection::EmptyMetaDescriptionUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaDescriptions = WebCrawler::DataCollection::DuplicatedMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryLongMetaDescriptions = WebCrawler::DataCollection::VeryLongMetaDescriptionUrlStorageType,
	StorageAdaptorTypeVeryShortMetaDescriptions = WebCrawler::DataCollection::VeryShortMetaDescriptionUrlStorageType,
	StorageAdaptorTypeSeveralMetaDescriptionsOnPage = WebCrawler::DataCollection::SeveralMetaDescriptionUrlStorageType,

	StorageAdaptorTypeEmptyMetaKeywords = WebCrawler::DataCollection::EmptyMetaKeywordsUrlStorageType,
	StorageAdaptorTypeDuplicatedMetaKeywords = WebCrawler::DataCollection::DuplicatedMetaKeywordsUrlStorageType,
	StorageAdaptorTypeSeveralMetaKeywordsOnPage = WebCrawler::DataCollection::SeveralMetaKeywordsUrlStorageType,

	StorageAdaptorTypeMissingH1s = WebCrawler::DataCollection::MissingH1UrlStorageType,
	StorageAdaptorTypeDuplicatedH1s = WebCrawler::DataCollection::DuplicatedH1UrlStorageType,
	StorageAdaptorTypeVeryLongH1s = WebCrawler::DataCollection::VeryLongH1UrlStorageType,
	StorageAdaptorTypeSeveralH1s = WebCrawler::DataCollection::SeveralH1UrlStorageType,

	StorageAdaptorTypeMissingH2s = WebCrawler::DataCollection::MissingH2UrlStorageType,
	StorageAdaptorTypeDuplicatedH2s = WebCrawler::DataCollection::DuplicatedH2UrlStorageType,
	StorageAdaptorTypeVeryLongH2s = WebCrawler::DataCollection::VeryLongH2UrlStorageType,
	StorageAdaptorTypeSeveralH2s = WebCrawler::DataCollection::SeveralH2UrlStorageType,

	StorageAdaptorTypeImagesOver100kb = WebCrawler::DataCollection::Over100kbImageStorageType,
	StorageAdaptorTypeImageMissingAltText = WebCrawler::DataCollection::MissingAltTextImageStorageType,
	StorageAdaptorTypeImagesVeryLongAltText = WebCrawler::DataCollection::VeryLongAltTextImageStorageType,

	// internal resources enum mappers
	StorageAdaptorTypeHtmlResources = WebCrawler::DataCollection::HtmlResourcesStorageType,
	StorageAdaptorTypeImageResources = WebCrawler::DataCollection::ImageResourcesStorageType,
	StorageAdaptorTypeJavaScriptResources = WebCrawler::DataCollection::JavaScriptResourcesStorageType,
	StorageAdaptorTypeStyleSheetResources = WebCrawler::DataCollection::StyleSheetResourcesStorageType,
	StorageAdaptorTypeFlashResources = WebCrawler::DataCollection::FlashResourcesStorageType,
	StorageAdaptorTypeVideoResources = WebCrawler::DataCollection::VideoResourcesStorageType,
	StorageAdaptorTypeOtherResources = WebCrawler::DataCollection::OtherResourcesStorageType,

	// external resources enum mappers
	StorageAdaptorTypeExternalHtmlResources = WebCrawler::DataCollection::ExternalHtmlResourcesStorageType,
	StorageAdaptorTypeExternalImageResources = WebCrawler::DataCollection::ExternalImageResourcesStorageType,
	StorageAdaptorTypeExternalJavaScriptResources = WebCrawler::DataCollection::ExternalJavaScriptResourcesStorageType,
	StorageAdaptorTypeExternalStyleSheetResources = WebCrawler::DataCollection::ExternalStyleSheetResourcesStorageType,
	StorageAdaptorTypeExternalFlashResources = WebCrawler::DataCollection::ExternalFlashResourcesStorageType,
	StorageAdaptorTypeExternalVideoResources = WebCrawler::DataCollection::ExternalVideoResourcesStorageType,
	StorageAdaptorTypeExternalOtherResources = WebCrawler::DataCollection::ExternalOtherResourcesStorageType,

	StorageAdaptorTypeEnd = WebCrawler::DataCollection::EndEnumStorageType
};

}
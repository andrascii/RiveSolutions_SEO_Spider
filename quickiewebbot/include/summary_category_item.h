#pragma once

#include "data_collection.h"

namespace QuickieWebBot
{

enum class SummaryCategoryItem
{
	SummaryCategoryItemBegin = WebCrawler::DataCollection::BeginEnumStorageType,

	SummaryCategoryItemNone,

	// used for headers
	SummaryCategoryItemHeader,

	// all these values must be able to mapped to DataCollection::StorageType

	SummaryCategoryItemAllPages = WebCrawler::DataCollection::CrawledUrlStorageType,

	SummaryCategoryItemUpperCaseLinks = WebCrawler::DataCollection::UpperCaseUrlStorageType,
	SummaryCategoryItemNonAsciiLinks = WebCrawler::DataCollection::NonAsciiCharacterUrlStorageType,
	SummaryCategoryItemVeryLongLinks = WebCrawler::DataCollection::VeryLongUrlStorageType,
	SummaryCategoryItemBrokenLinks = WebCrawler::DataCollection::Status404StorageType,

	SummaryCategoryItemEmptyTitles = WebCrawler::DataCollection::EmptyTitleUrlStorageType,
	SummaryCategoryItemDuplicatedTitles = WebCrawler::DataCollection::DuplicatedTitleUrlStorageType,
	SummaryCategoryItemVeryLongTitles = WebCrawler::DataCollection::VeryLongTitleUrlStorageType,
	SummaryCategoryItemVeryShortTitles = WebCrawler::DataCollection::VeryShortTitleUrlStorageType,
	SummaryCategoryItemDuplicatedH1Titles = WebCrawler::DataCollection::DuplicatedH1TitleUrlStorageType,
	SummaryCategoryItemSeveralTitlesOnPage = WebCrawler::DataCollection::SeveralTitleUrlStorageType,

	SummaryCategoryItemEmptyMetaDescriptions = WebCrawler::DataCollection::EmptyMetaDescriptionUrlStorageType,
	SummaryCategoryItemDuplicatedMetaDescriptions = WebCrawler::DataCollection::DuplicatedMetaDescriptionUrlStorageType,
	SummaryCategoryItemVeryLongMetaDescriptions = WebCrawler::DataCollection::VeryLongMetaDescriptionUrlStorageType,
	SummaryCategoryItemVeryShortMetaDescriptions = WebCrawler::DataCollection::VeryShortMetaDescriptionUrlStorageType,
	SummaryCategoryItemSeveralMetaDescriptionsOnPage = WebCrawler::DataCollection::SeveralMetaDescriptionUrlStorageType,

	SummaryCategoryItemEmptyMetaKeywords = WebCrawler::DataCollection::EmptyMetaKeywordsUrlStorageType,
	SummaryCategoryItemDuplicatedMetaKeywords = WebCrawler::DataCollection::DuplicatedMetaKeywordsUrlStorageType,
	SummaryCategoryItemSeveralMetaKeywordsOnPage = WebCrawler::DataCollection::SeveralMetaKeywordsUrlStorageType,

	SummaryCategoryItemMissingH1s = WebCrawler::DataCollection::MissingH1UrlStorageType,
	SummaryCategoryItemDuplicatedH1s = WebCrawler::DataCollection::DuplicatedH1UrlStorageType,
	SummaryCategoryItemVeryLongH1s = WebCrawler::DataCollection::VeryLongH1UrlStorageType,
	SummaryCategoryItemSeveralH1s = WebCrawler::DataCollection::SeveralH1UrlStorageType,

	SummaryCategoryItemMissingH2s = WebCrawler::DataCollection::MissingH2UrlStorageType,
	SummaryCategoryItemDuplicatedH2s = WebCrawler::DataCollection::DuplicatedH2UrlStorageType,
	SummaryCategoryItemVeryLongH2s = WebCrawler::DataCollection::VeryLongH2UrlStorageType,
	SummaryCategoryItemSeveralH2s = WebCrawler::DataCollection::SeveralH2UrlStorageType,

	SummaryCategoryItemImagesOver100kb = WebCrawler::DataCollection::Over100kbImageStorageType,
	SummaryCategoryItemImageMissingAltText = WebCrawler::DataCollection::MissingAltTextImageStorageType,
	SummaryCategoryItemImagesVeryLongAltText = WebCrawler::DataCollection::VeryLongAltTextImageStorageType,

	SummaryCategoryItemHtmlResources = WebCrawler::DataCollection::HtmlResourcesStorageType,
	SummaryCategoryItemImageResources = WebCrawler::DataCollection::ImageResourcesStorageType,
	SummaryCategoryItemJavaScriptResources = WebCrawler::DataCollection::JavaScriptResourcesStorageType,
	SummaryCategoryItemStyleSheetResources = WebCrawler::DataCollection::StyleSheetResourcesStorageType,
	SummaryCategoryItemFlashResources = WebCrawler::DataCollection::FlashResourcesStorageType,
	SummaryCategoryItemVideoResources = WebCrawler::DataCollection::VideoResourcesStorageType,
	SummaryCategoryItemOtherResources = WebCrawler::DataCollection::OtherResourcesStorageType,

	SummaryCategoryItemEnd = WebCrawler::DataCollection::EndEnumStorageType
};

}
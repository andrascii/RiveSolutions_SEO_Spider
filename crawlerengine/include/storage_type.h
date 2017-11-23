#pragma once

namespace CrawlerEngine
{

enum StorageType
{
	BeginEnumStorageType,
	// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

	//
	// Statistic data
	//
	CrawledUrlStorageType,
	ExternalUrlStorageType,

	//
	// Url problems
	//
	UpperCaseUrlStorageType,
	NonAsciiCharacterUrlStorageType,
	VeryLongUrlStorageType,

	//
	// Title problems
	//
	AllTitlesUrlStorageType,
	EmptyTitleUrlStorageType,
	DuplicatedTitleUrlStorageType,
	VeryLongTitleUrlStorageType,
	VeryShortTitleUrlStorageType,
	DuplicatedH1TitleUrlStorageType,
	SeveralTitleUrlStorageType,

	//
	// Meta description problems
	//
	AllMetaDescriptionsUrlStorageType,
	EmptyMetaDescriptionUrlStorageType,
	DuplicatedMetaDescriptionUrlStorageType,
	VeryLongMetaDescriptionUrlStorageType,
	VeryShortMetaDescriptionUrlStorageType,
	SeveralMetaDescriptionUrlStorageType,

	//
	// Meta keywords problems
	//
	AllMetaKeywordsUrlStorageType,
	EmptyMetaKeywordsUrlStorageType,
	DuplicatedMetaKeywordsUrlStorageType,
	SeveralMetaKeywordsUrlStorageType,

	//
	// H1 problems
	//
	AllH1UrlStorageType,
	MissingH1UrlStorageType,
	DuplicatedH1UrlStorageType,
	VeryLongH1UrlStorageType,
	SeveralH1UrlStorageType,

	//
	// H2 problems
	//
	AllH2UrlStorageType,
	MissingH2UrlStorageType,
	DuplicatedH2UrlStorageType,
	VeryLongH2UrlStorageType,
	SeveralH2UrlStorageType,

	//
	// Images problems
	//
	Over100kbImageStorageType,
	MissingAltTextImageStorageType,
	VeryLongAltTextImageStorageType,

	//
	// Statuses
	//
	Status404StorageType,


	//
	// Resources
	//
	PendingResourcesStorageType,
	CanonicalResourcesStorageType,
	HtmlResourcesStorageType,
	ImageResourcesStorageType,
	JavaScriptResourcesStorageType,
	StyleSheetResourcesStorageType,
	FlashResourcesStorageType,
	VideoResourcesStorageType,
	OtherResourcesStorageType,

	//
	// ExternalResources
	//
	ExternalHtmlResourcesStorageType,
	ExternalImageResourcesStorageType,
	ExternalJavaScriptResourcesStorageType,
	ExternalStyleSheetResourcesStorageType,
	ExternalFlashResourcesStorageType,
	ExternalVideoResourcesStorageType,
	ExternalOtherResourcesStorageType,

	// !!!!!!!!!!!!!!!!!!! add new items above this!!!!!!!!!!!!!!!!!!!
	EndEnumStorageType
};

}
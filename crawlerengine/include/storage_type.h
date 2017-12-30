#pragma once

namespace CrawlerEngine
{

enum class StorageType
{
	BeginEnumStorageType,
	// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

	// Statistic data
	CrawledUrlStorageType,
	ExternalUrlStorageType,

	// Url problems
	UpperCaseUrlStorageType,
	NonAsciiCharacterUrlStorageType,
	TooLongUrlStorageType,
	BrokenLinks,
	Status4xxStorageType,
	Status5xxStorageType,
	Status302StorageType,
	Status301StorageType,

	// Title problems
	AllTitlesUrlStorageType,
	EmptyTitleUrlStorageType,
	DuplicatedTitleUrlStorageType,
	TooLongTitleUrlStorageType,
	TooShortTitleUrlStorageType,
	DuplicatedH1TitleUrlStorageType,
	SeveralTitleUrlStorageType,

	// Meta description problems
	AllMetaDescriptionsUrlStorageType,
	EmptyMetaDescriptionUrlStorageType,
	DuplicatedMetaDescriptionUrlStorageType,
	TooLongMetaDescriptionUrlStorageType,
	TooShortMetaDescriptionUrlStorageType,
	SeveralMetaDescriptionUrlStorageType,

	// Meta keywords problems
	AllMetaKeywordsUrlStorageType,
	EmptyMetaKeywordsUrlStorageType,
	DuplicatedMetaKeywordsUrlStorageType,
	SeveralMetaKeywordsUrlStorageType,

	// H1 problems
	AllH1UrlStorageType,
	MissingH1UrlStorageType,
	DuplicatedH1UrlStorageType,
	TooLongH1UrlStorageType,
	SeveralH1UrlStorageType,

	// H2 problems
	AllH2UrlStorageType,
	MissingH2UrlStorageType,
	DuplicatedH2UrlStorageType,
	TooLongH2UrlStorageType,
	SeveralH2UrlStorageType,

	// Images problems
	Over100kbImageStorageType,
	MissingAltTextImageStorageType,
	TooLongAltTextImageStorageType,

	// page problems
	TooManyLinksOnPageStorageType,
	ContainsMetaRefreshTagStorageType,
	ContainsFramesStorageType,

	// Resources
	PendingResourcesStorageType,
	CanonicalResourcesStorageType,
	HtmlResourcesStorageType,
	ImageResourcesStorageType,
	JavaScriptResourcesStorageType,
	StyleSheetResourcesStorageType,
	FlashResourcesStorageType,
	VideoResourcesStorageType,
	OtherResourcesStorageType,

	// ExternalResources
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

Q_DECLARE_METATYPE(CrawlerEngine::StorageType)
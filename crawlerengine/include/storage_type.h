#pragma once

namespace CrawlerEngine
{

enum StorageType
{
	BeginEnumStorageType,
	// !!!!!!!!!!!!!!!!!!! add new items below this!!!!!!!!!!!!!!!!!!!

	// Statistic data
	CrawledUrlStorageType,
	ExternalUrlStorageType,
	DofollowUrlStorageType,

	// Url problems
	UpperCaseUrlStorageType,
	NonAsciiCharacterUrlStorageType,
	TooLongUrlStorageType,
	BrokenLinks,
	WwwRedirectionsUrlStorageType,
	TooManyRedirectsStorageType,
	Status4xxStorageType,
	Status5xxStorageType,
	Status302StorageType,
	Status301StorageType,
	TimeoutStorageType,

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
	TooBigImageStorageType,
	MissingAltTextImageStorageType,
	TooLongAltTextImageStorageType,
	BrokenImagesStorageType,

	// page problems
	TooManyLinksOnPageStorageType,
	ContainsMetaRefreshTagStorageType,
	ContainsFramesStorageType,
	TooBigHtmlResourcesStorageType,

	// not indexed pages
	BlockedForSEIndexingStorageType,
	NofollowLinksStorageType,
	BlockedByRobotsTxtStorageType,
	BlockedByXRobotsTagStorageType,

	// Resources
	PendingResourcesStorageType,
	HtmlResourcesStorageType,
	ImageResourcesStorageType,
	JavaScriptResourcesStorageType,
	StyleSheetResourcesStorageType,
	FlashResourcesStorageType,
	VideoResourcesStorageType,
	OtherResourcesStorageType,
	AllCanonicalUrlResourcesStorageType,
	DuplicatedCanonicalUrlResourcesStorageType,
	UniqueCanonicalUrlResourcesStorageType,
	ExternalDoFollowUrlResourcesStorageType,

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

inline StorageType operator++(StorageType value)
{
	if (value == StorageType::EndEnumStorageType)
	{
		return StorageType::BeginEnumStorageType;
	}

	unsigned int x = static_cast<unsigned int>(value);
	return static_cast<StorageType>(++x);
}

}

Q_DECLARE_METATYPE(CrawlerEngine::StorageType)
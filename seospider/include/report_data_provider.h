#pragma once

#include "error_category.h"

namespace CrawlerEngine
{

class SequencedDataCollection;

}

namespace SeoSpider
{

enum class ReportDataKeys
{
	// report header
	SiteShortImage,
	SiteLink,
	FoundProblems,
	FoundProblemsCount,
	Errors,
	ErrorsImage,
	ErrorsCount,
	Warnings,
	WarningsImage,
	WarningsCount,
	Info,
	InfoImage,
	InfoCount,
	Date,
	DateImage,

	// Signature
	CompanyName,
	CompanyEmail,
	CompanyWebSite,


	// Indexing and Page Scanning
	StatusCode4xx,
	StatusCode4xxImage,
	StatusCode4xxCount,
	StatusCode5xx,
	StatusCode5xxImage,
	StatusCode5xxCount,
	NotIndexedPages,
	NotIndexedPagesImage,
	NotIndexedPagesCount,
	ConfiguredCorrectly404,
	ConfiguredCorrectly404Image,
	ConfiguredCorrectly404Count,
	RobotsTxt,
	RobotsTxtImage,
	RobotsTxtCount,
	XmlSitemap,
	XmlSitemapImage,
	XmlSitemapCount,

	// Redirections
	WwwFixedVersion,
	WwwFixedVersionImage,
	WwwFixedVersionCount,
	Redirections302,
	Redirections302Image,
	Redirections302Count,
	Redirections301,
	Redirections301Image,
	Redirections301Count,
	LargeAmountRedirects,
	LargeAmountRedirectsImage,
	LargeAmountRedirectsCount,
	RefreshMetaTag,
	RefreshMetaTagImage,
	RefreshMetaTagCount,
	RelCanonicalPages,
	RelCanonicalPagesImage,
	RelCanonicalPagesCount,

	// Links
	BrokenLinks,
	BrokenLinksImage,
	BrokenLinksCount,
	PagesWithLargeAmountOfLinks,
	PagesWithLargeAmountOfLinksImage,
	PagesWithLargeAmountOfLinksCount,
	ExternalDofollowLinks,
	ExternalDofollowLinksImage,
	ExternalDofollowLinksCount,
	TooLongUrlAddresses,
	TooLongUrlAddressesImage,
	TooLongUrlAddressesCount,

	// Technical factors
	PagesWithDuplicatedRelCanonical,
	PagesWithDuplicatedRelCanonicalImage,
	PagesWithDuplicatedRelCanonicalCount,
	PagesContainsFrames,
	PagesContainsFramesImage,
	PagesContainsFramesCount,
	PagesWithHtmlErrors,
	PagesWithHtmlErrorsImage,
	PagesWithHtmlErrorsCount,
	PagesWithCssErrorsWarnings,
	PagesWithCssErrorsWarningsImage,
	PagesWithCssErrorsWarningsCount,
	TooLargePages,
	TooLargePagesImage,
	TooLargePagesCount,

	// Images
	BrokenImages,
	BrokenImagesImage,
	BrokenImagesCount,
	ImagesWithEmptyAltText,
	ImagesWithEmptyAltTextImage,
	ImagesWithEmptyAltTextCount,

	// On Page
	EmptyTitles,
	EmptyTitlesImage,
	EmptyTitlesCount,
	DuplicatedTitles,
	DuplicatedTitlesImage,
	DuplicatedTitlesCount,
	TooLongTitles,
	TooLongTitlesImage,
	TooLongTitlesCount,
	EmptyMetaDescriptions,
	EmptyMetaDescriptionsImage,
	EmptyMetaDescriptionsCount,
	DuplicatedMetaDescriptions,
	DuplicatedMetaDescriptionsImage,
	DuplicatedMetaDescriptionsCount,
	TooLongMetaDescriptions,
	TooLongMetaDescriptionsImage,
	TooLongMetaDescriptionsCount
};

class ReportDataProvider
{
public:
	ReportDataProvider(CrawlerEngine::SequencedDataCollection* sequencedDataCollection);

	QVariant data(ReportDataKeys dataKey) const;

	QList<ReportDataKeys> allKeys() const;

	QByteArray placeholder(ReportDataKeys reportDataKey) const noexcept;

private:
	const CrawlerEngine::SequencedDataCollection* m_sequencedDataCollection;

	QMap<CrawlerEngine::ErrorCategory::ErrorCategoryLevel, QPixmap> m_pixmaps;
};

}
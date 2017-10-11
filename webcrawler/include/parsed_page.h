#pragma once

#include "status_code.h"

namespace WebCrawler
{

struct ParsedPage;

using ParsedPageWeakPtr = std::weak_ptr<ParsedPage>;


enum class LinkParameter
{
	UnknownParameter,
	NofollowParameter,
	DofollowParameter
};

enum class ResourceType
{
	ResourceHtml,
	ResourceImage,
	ResourceJavaScript,
	ResourceStyleSheet,
	ResourceFlash,
	ResourceVideo,
	ResourceOther
};

enum class ResourceSource
{
	SourceInvalid,
	SourceTagA,
	SourceTagImg,
	SourceTagLink,
	SourceTagScript,
	SourceCSS
};

struct LinkInfo
{
	QUrl url;
	LinkParameter urlParameter;
	QString altOrTitle;
};

struct ResourceLink
{
	ParsedPageWeakPtr resource;
	LinkParameter urlParameter;
	ResourceSource resourceSource;
	QString altOrTitle;
};

struct RawResourceOnPage
{
	ResourceType resourceType;
	LinkInfo thisResourceLink;
	ResourceSource resourceSource;
};

struct ParsedPage
{
	QUrl url;
	QUrl redirectedUrl;

	QString title;
	QString contentType;
	QString metaRefresh;
	QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	QString canonicalLinkElement;
	Common::StatusCode statusCode;

	int pageSizeKilobytes = int();
	int wordCount = int();
	size_t pageHash = size_t();

	std::vector<size_t> missignAltIndices;
	std::vector<size_t> tooLongAltIndices;

	bool hasSeveralTitleTags = bool();
	bool hasSeveralMetaDescriptionTags = bool();
	bool hasSeveralMetaKeywordsTags = bool();
	bool hasSeveralEqualH1Tags = bool();
	bool hasSeveralEqualH2Tags = bool();

	bool isThisExternalPage = bool();

	ResourceType resourceType;

	std::deque<RawResourceOnPage> allResourcesOnPage;
	std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

	QByteArray rawResponse;
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr);
Q_DECLARE_METATYPE(ParsedPageWeakPtr);

}
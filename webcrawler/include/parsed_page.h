#pragma once

#include "status_code.h"

namespace WebCrawler
{

struct ParsedPage;

using ParsedPageWeakPtr = std::weak_ptr<ParsedPage>;


enum class LinkParameter
{
	DofollowParameter,
	NofollowParameter
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

enum MetaRobotsItem
{
	MetaRobotsEmpty = 0,
	MetaRobotsAll = 1,
	MetaRobotsNoIndex = 1 << 1,
	MetaRobotsNoFollow = 1 << 2,
	MetaRobotsNone = 1 << 3,
	MetaRobotsNoArchive = 1 << 4,
	MetaRobotsNoSnippet = 1 << 5,
	MetaRobotsNoODP = 1 << 6,
	MetaRobotsNoTranslate = 1 << 7,
	MetaRobotsNoImageIndex = 1 << 8,
	MetaRobotsIndex = 1 << 9,
	MetaRobotsFollow = 1 << 10
};

Q_DECLARE_FLAGS(MetaRobotsFlags, MetaRobotsItem);

struct LinkInfo
{
	QUrl url;
	LinkParameter urlParameter = LinkParameter::DofollowParameter;
	QString altOrTitle;
	bool dataResourceLink = false;
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

inline bool operator<(const RawResourceOnPage& lhs, const RawResourceOnPage& rhs)
{
	return lhs.thisResourceLink.url.toDisplayString() < rhs.thisResourceLink.url.toDisplayString();
}

struct ParsedPage
{
	QUrl url;
	QUrl redirectedUrl;

	QString title;
	QString contentType;
	QString metaRefresh;
	//QString metaRobots;
	QString metaDescription;
	QString metaKeywords;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	QString canonicalLinkElement;
	Common::StatusCode statusCode;
	MetaRobotsFlags metaRobotsFlags = MetaRobotsEmpty;

	int pageSizeKilobytes = int();
	int wordCount = int();
	size_t pageHash = size_t();

	std::vector<size_t> missignAltIndices;
	std::vector<size_t> tooLongAltIndices;

	bool hasSeveralTitleTags = bool();
	bool hasSeveralMetaDescriptionTags = bool();
	bool hasSeveralMetaKeywordsTags = bool();
	bool hasSeveralH1Tags = bool();
	bool hasSeveralEqualH2Tags = bool();

	bool isThisExternalPage = bool();

	ResourceType resourceType;

	std::set<RawResourceOnPage> allResourcesOnPage;
	std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

	QByteArray rawResponse;
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr);
Q_DECLARE_METATYPE(ParsedPageWeakPtr);

}

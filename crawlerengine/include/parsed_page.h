#pragma once

#include "status_code.h"

namespace CrawlerEngine
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
	SourceTagLinkRelCanonical,
	SourceTagLinkRelNext,
	SourceTagLinkRelPrev,
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
	MetaRobotsFollow = 1 << 10,
	MetaRobotsNoYaCa = 1 << 11,
	MetaRobotsNoYDir = 1 << 12
};

Q_DECLARE_FLAGS(MetaRobotsFlags, MetaRobotsItem);

enum class UserAgentType
{
	Unknown,
	GoogleBot,
	YandexBot,
	MailRuBot,
	YahooBot,
	MsnBot,
	AltaVistaBot,
	RamblerBot,
	AportBot,
	WebAltaBot,
	AnyBot // used for all robots
};

using MetaRobotsFlagsSet = std::map<UserAgentType, MetaRobotsFlags>;

struct LinkInfo
{
	QUrl url;
	LinkParameter urlParameter = LinkParameter::DofollowParameter;
	QString altOrTitle;
	bool dataResourceLink = false;
	ResourceSource resourceSource;
};

struct ResourceLink
{
	ParsedPageWeakPtr resource;
	LinkParameter linkParameter;
	ResourceSource resourceSource;
	QString altOrTitle;
};

struct RawResourceOnPage
{
	ResourceType resourceType;
	LinkInfo thisResourceLink;
	//ResourceSource resourceSource;
};

inline bool operator<(const RawResourceOnPage& lhs, const RawResourceOnPage& rhs)
{
	return lhs.thisResourceLink.url.toDisplayString() < rhs.thisResourceLink.url.toDisplayString();
}

constexpr int invalidPageLevel = 100000000;

struct ParsedPage
{
	QUrl url;
	QUrl redirectedUrl;
	QUrl canonicalUrl;

	QString title;
	QString contentType;
	QString metaRefresh;
	QString metaDescription;
	QString metaKeywords;
	QString serverResponse;
	QString firstH1;
	QString secondH1;
	QString firstH2;
	QString secondH2;
	Common::StatusCode statusCode;
	MetaRobotsFlagsSet metaRobotsFlags;
	QDateTime responseDate;
	QDateTime lastModifiedDate;

	int pageSizeKilobytes = int();
	int wordCount = int();
	size_t pageHash = size_t();

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
	int pageLevel = invalidPageLevel;
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr);
Q_DECLARE_METATYPE(ParsedPageWeakPtr);

}

#pragma once

#include "status_code.h"
#include "url.h"

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
	SourceTagMetaRefresh,
	SourceCSS,
	SourceRedirectUrl
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
	Url url;
	LinkParameter urlParameter = LinkParameter::DofollowParameter;
	QString altOrTitle;
	bool dataResourceLink = false;
	ResourceSource resourceSource;
};

struct ResourceLink
{
	ParsedPageWeakPtr resource;
	Url url;
	LinkParameter linkParameter;
	ResourceSource resourceSource;
	QString altOrTitle;
};

struct ResourceOnPage
{
	ResourceOnPage(ResourceType resourceType, const LinkInfo& linkInfo, bool loadAvailability = true)
		: resourceType(resourceType)
		, link(linkInfo)
		, loadAvailability(loadAvailability)
	{
	}

	ResourceOnPage(ResourceType resourceType, LinkInfo&& linkInfo, bool loadAvailability = true)
		: resourceType(resourceType)
		, link(std::move(linkInfo))
		, loadAvailability(loadAvailability)
	{
	}

	ResourceType resourceType;
	LinkInfo link;
	bool loadAvailability;
};

struct ResourcesOnPageListItemHasher
{
	size_t operator()(const ResourceOnPage& resource) const noexcept
	{
		return hasher(resource.link.url.urlStr().toStdString());
		// TODO: make it more clever
	}

	boost::hash<std::string> hasher;
};

inline bool operator==(const ResourceOnPage& first, const ResourceOnPage& second)
{
	return first.link.url == second.link.url;
}

using ResourcesOnPageList = std::unordered_set<ResourceOnPage, ResourcesOnPageListItemHasher>;

inline bool operator<(const ResourceOnPage& lhs, const ResourceOnPage& rhs)
{
	return lhs.link.url.urlStr() < rhs.link.url.urlStr();
}

constexpr int invalidPageLevel = 100000000;

struct ParsedPage
{
	Url url;
	Url redirectedUrl;
	Url canonicalUrl;

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
	int secondsToRefresh = -1;
	size_t pageHash = size_t();

	bool hasSeveralTitleTags = bool();
	bool hasSeveralMetaDescriptionTags = bool();
	bool hasSeveralMetaKeywordsTags = bool();
	bool hasSeveralH1Tags = bool();
	bool hasSeveralEqualH2Tags = bool();
	bool hasMetaRefreshTag = bool();
	bool hasFrames = bool();
	bool isThisExternalPage = bool();

	ResourceType resourceType;
	ResourcesOnPageList allResourcesOnPage;
	std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

	QByteArray rawResponse;

	int pageLevel = invalidPageLevel;

	std::vector<bool> storages;
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr);
Q_DECLARE_METATYPE(std::vector<ParsedPagePtr>);
Q_DECLARE_METATYPE(ParsedPageWeakPtr);
Q_DECLARE_METATYPE(UserAgentType);

}

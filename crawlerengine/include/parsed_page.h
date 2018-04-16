#pragma once

#include "status_code.h"
#include "url.h"

namespace CrawlerEngine
{


#ifndef SERIALIZED
# define SERIALIZED ;
#endif

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

Q_DECLARE_FLAGS(MetaRobotsFlags, MetaRobotsItem)

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

enum Restriction
{
	RestrictionExternalLinksNotAllowed = 1,
	RestrictionNofollowNotAllowed = 1 << 1,
	RestrictionBlockedByRobotsTxtRules = 1 << 2,
	RestrictionBlockedByMetaRobotsRules = 1 << 3,
	RestrictionSubdomainNotAllowed = 1 << 4,
	RestrictionBlockedByFolder = 1 << 5,
	RestrictionNotHttpLinkNotAllowed = 1 << 6
};

Q_DECLARE_FLAGS(RestrictionFlags, Restriction)

struct LinkInfo
{
	Url url;
	LinkParameter linkParameter = LinkParameter::DofollowParameter;
	QString altOrTitle;
	bool dataResourceLink = false;
	ResourceSource resourceSource;
};

struct ResourceLink
{
	ParsedPageWeakPtr resource;
	SERIALIZED Url url;
	SERIALIZED LinkParameter linkParameter;
	SERIALIZED ResourceSource resourceSource;
	SERIALIZED QString altOrTitle;
};

struct ResourceOnPage
{
	ResourceOnPage(ResourceType resourceType, const LinkInfo& linkInfo, 
		RestrictionFlags restrictions = RestrictionFlags())
		: resourceType(resourceType)
		, link(linkInfo)
		, restrictions(restrictions)
	{
	}

	ResourceOnPage(ResourceType resourceType, LinkInfo&& linkInfo, 
		RestrictionFlags restrictions = RestrictionFlags())
		: resourceType(resourceType)
		, link(std::move(linkInfo))
		, restrictions(restrictions)
	{
	}

	ResourceType resourceType;
	LinkInfo link;
	RestrictionFlags restrictions;
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
	SERIALIZED Url url;
	SERIALIZED Url redirectedUrl;
	SERIALIZED Url canonicalUrl;

	SERIALIZED QString title;
	SERIALIZED QString contentType;
	SERIALIZED QString metaRefresh;
	SERIALIZED QString metaDescription;
	SERIALIZED QString metaKeywords;
	SERIALIZED QString serverResponse;
	SERIALIZED QString firstH1;
	SERIALIZED QString secondH1;
	SERIALIZED QString firstH2;
	SERIALIZED QString secondH2;
	SERIALIZED Common::StatusCode statusCode;
	SERIALIZED MetaRobotsFlagsSet metaRobotsFlags;
	SERIALIZED QDateTime responseDate;
	SERIALIZED QDateTime lastModifiedDate;

	SERIALIZED int pageSizeKilobytes = int();
	SERIALIZED int wordCount = int();
	SERIALIZED int secondsToRefresh = -1;
	SERIALIZED size_t pageHash = size_t();

	SERIALIZED bool hasSeveralTitleTags = bool();
	SERIALIZED bool hasSeveralMetaDescriptionTags = bool();
	SERIALIZED bool hasSeveralMetaKeywordsTags = bool();
	SERIALIZED bool hasSeveralH1Tags = bool();
	SERIALIZED bool hasSeveralEqualH2Tags = bool();
	SERIALIZED bool hasMetaRefreshTag = bool();
	SERIALIZED bool hasFrames = bool();
	SERIALIZED bool isThisExternalPage = bool();
	SERIALIZED bool isBlockedForIndexing = bool();
	SERIALIZED bool isBlockedByMetaRobots = bool();
	SERIALIZED bool tooManyRedirects = bool();

	SERIALIZED ResourceType resourceType;
	ResourcesOnPageList allResourcesOnPage;
	SERIALIZED std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

	SERIALIZED QByteArray rawResponse;

	SERIALIZED int pageLevel = invalidPageLevel;

	SERIALIZED std::vector<bool> storages;

	bool canRefresh() const noexcept
	{
		return Common::StatusCode::BlockedByRobotsTxt != statusCode &&
			Common::StatusCode::BlockedByRobotsTxt != statusCode;
	}
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr)
Q_DECLARE_METATYPE(std::vector<ParsedPagePtr>)
Q_DECLARE_METATYPE(ParsedPageWeakPtr)
Q_DECLARE_METATYPE(UserAgentType)

}

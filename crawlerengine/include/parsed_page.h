#pragma once
#include "stdafx.h"

#include "status_code.h"
#include "url.h"
#include "storage_type.h"

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
	SourceTagLinkAlternateHrefLang,
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

enum class DataFeedId
{
	YandexMetrica = 0,
	GoogleAnalytics = 1,
	TestDataFeed = 1000
};

enum class PageCounterId
{
	YandexMetrica = 1 << 1,
	GoogleAnalytics = 1 << 2
};

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
	Url url;
	LinkParameter linkParameter;
	ResourceSource resourceSource;
	QString altOrTitle;
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
		return qHash(resource.link.url);
	}
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
	bool canRefresh() const noexcept
	{
		return Common::StatusCode::BlockedByRobotsTxt != statusCode &&
			Common::StatusCode::BlockedByRobotsTxt != statusCode;
	}

	bool isRedirectedToExternalPage() const
	{
		return redirectedUrl.isValid() && isThisExternalPage;
	}

	Url url; // url address of a page
	Url baseUrl; // url parsed from <base href="https://website.com/">
	Url redirectedUrl; // redirection url received from http response header "location" or from meta refresh tag
	Url canonicalUrl; // canonical address

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
	bool isBlockedByMetaRobots = bool();

	ResourceType resourceType;

	// this container used by workers and finally ModelController clears it
	// TODO: this intermediate data must be transfer using another way
	ResourcesOnPageList allResourcesOnPage;

	std::deque<ResourceLink> linksOnThisPage;
	std::deque<ResourceLink> linksToThisPage;

	QByteArray rawResponse;

	int pageLevel = invalidPageLevel;
	std::vector<bool> storages;

	QMap<DataFeedId, QMap<int, QString>> dataFeedsData;
	std::vector<StorageType> missingYandexMetricaCounters;
	std::vector<StorageType> missingGoogleAnalyticsCounters;

	//! response time in milliseconds
	int responseTime = int();

#ifdef QT_DEBUG
	bool headRequest = false;
#endif
};

using ParsedPagePtr = std::shared_ptr<ParsedPage>;

Q_DECLARE_METATYPE(ParsedPagePtr)
Q_DECLARE_METATYPE(std::vector<ParsedPagePtr>)
Q_DECLARE_METATYPE(ParsedPageWeakPtr)
Q_DECLARE_METATYPE(UserAgentType)

}

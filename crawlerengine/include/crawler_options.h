#pragma once

#include "robots_txt_rules.h"

namespace CrawlerEngine
{

enum ParserType
{
	JavaScriptResourcesParserType = 1 << 0,
	CssResourcesParserType = 1 << 1,
	ImagesResourcesParserType = 1 << 2,
	VideoResourcesParserType = 1 << 3,
	FlashResourcesParserType = 1 << 4,
	OtherResourcesParserType = 1 << 5
};

Q_DECLARE_FLAGS(ParserTypeFlags, ParserType);

struct CrawlerOptions
{
	SERIALIZED Url startCrawlingPage;
	
	SERIALIZED int limitMaxUrlLength = int();
	SERIALIZED int limitSearchTotal = int();
	SERIALIZED int limitTimeout = int();
	SERIALIZED int maxRedirectsToFollow = int();
	SERIALIZED int maxLinksCountOnPage = int();

	// preferences
	SERIALIZED int maxTitleLength = int();
	SERIALIZED int minTitleLength = int();
	SERIALIZED int maxDescriptionLength = int();
	SERIALIZED int minDescriptionLength = int();
	SERIALIZED int maxH1LengthChars = int();
	SERIALIZED int maxH2LengthChars = int();
	SERIALIZED int maxImageAltTextChars = int();
	SERIALIZED int maxImageSizeKb = int();
	SERIALIZED int maxPageSizeKb = int();

	// proxy settings
	SERIALIZED bool useProxy = bool();
	SERIALIZED QString proxyHostName;
	SERIALIZED int proxyPort = int();
	SERIALIZED QString proxyUser;
	SERIALIZED QString proxyPassword;

	// crawler settings
	SERIALIZED bool checkExternalLinks = bool();
	SERIALIZED bool followInternalNofollow = bool();
	SERIALIZED bool followExternalNofollow = bool();
	SERIALIZED bool checkCanonicals = bool();
	SERIALIZED bool checkSubdomains = bool();
	SERIALIZED bool crawlOutsideOfStartFolder = bool();

	// robots.txt rules
	SERIALIZED bool followRobotsTxtRules = bool();
	SERIALIZED UserAgentType userAgentToFollow;

	SERIALIZED ParserTypeFlags parserTypeFlags;

	// pause between requests
	SERIALIZED int pauseRangeFrom = int();
	SERIALIZED int pauseRangeTo = int();

	// user agent
	SERIALIZED QByteArray userAgent;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
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
	Url host;
	
	int limitMaxUrlLength = int();
	int limitSearchTotal = int();
	int limitTimeout = int();
	int maxRedirectsToFollow = int();
	int maxLinksCountOnPage = int();

	// preferences
	int minTitleLength = int();
	int maxTitleLength = int();
	int maxDescriptionLength = int();
	int minDescriptionLength = int();
	int maxH1LengthChars = int();
	int maxH2LengthChars = int();
	int maxImageAltTextChars = int();
	int maxImageSizeKb = int();

	// crawler settings
	bool checkExternalLinks = bool();
	bool followInternalNofollow = bool();
	bool followExternalNofollow = bool();
	bool checkCanonicals = bool();
	bool checkSubdomains = bool();
	bool crawlOutsideOfStartFolder = bool();

	// robots.txt rules
	bool followRobotsTxtRules = bool();
	UserAgentType userAgentToFollow;

	ParserTypeFlags parserTypeFlags;

	// pause between requests
	int pauseRangeFrom = int();
	int pauseRangeTo = int();

	// user agent
	QByteArray userAgent;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
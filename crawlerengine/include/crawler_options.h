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
	CustomUrl host;
	
	int limitMaxUrlLength = int();

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
	bool checkExternalLinks = int();
	bool followInternalNofollow = int();
	bool followExternalNofollow = int();
	bool checkSubdomains = int();
	bool checkImages = int();
	bool checkCss = int();
	bool checkJavaScript = int();
	bool checkSwf = int();

	// robots.txt rules
	bool followRobotsTxtRules = bool();
	UserAgentType userAgentToFollow;

	// user agent
	QByteArray plainUserAgent;

	ParserTypeFlags parserTypeFlags;

	// pause between requests
	int pauseRangeFrom = int();
	int pauseRangeTo = int();
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
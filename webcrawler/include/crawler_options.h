#pragma once

#include "robots_txt_rules.h"

namespace WebCrawler
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
	QUrl host;
	
	int limitMaxUrlLength;

	// preferences
	int minTitleLength;
	int maxTitleLength;
	int maxDescriptionLength;
	int minDescriptionLength;
	int maxH1LengthChars;
	int maxH2LengthChars;
	int maxImageAltTextChars;
	int maxImageSizeKb;

	// crawler settings
	bool checkExternalLinks;
	bool followInternalNofollow;
	bool followExternalNofollow;
	bool checkSubdomains;
	bool checkImages;
	bool checkCss;
	bool checkJavaScript;
	bool checkSwf;

	// robots.txt rules
	bool followRobotsTxtRules;
	UserAgentType userAgentToFollow;

	// user agent
	QByteArray plainUserAgent;

	ParserTypeFlags parserTypeFlags;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
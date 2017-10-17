#pragma once

namespace WebCrawler
{

enum ParserType
{
	HtmlResourcesParserType = 1 << 0,
	JavaScriptResourcesParserType = 1 << 1,
	CssResourcesParserType = 1 << 2,
	ImagesResourcesParserType = 1 << 3,
	VideoResourcesParserType = 1 << 4,
	FlashResourcesParserType = 1 << 5
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
	bool followRobotsTxtRules;
	bool checkImages;
	bool checkCss;
	bool checkJavaScript;
	bool checkSwf;

	ParserTypeFlags parserTypeFlags;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
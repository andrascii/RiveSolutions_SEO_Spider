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
	FlashResourcesParserType = 1 << 5,
	OtherResourcesParserType = 1 << 6
};

Q_DECLARE_FLAGS(ParserTypeFlags, ParserType);

struct CrawlerOptions
{
	QUrl host;
	
	int minTitleLength;
	int maxTitleLength;

	int limitMaxUrlLength;

	int maxDescriptionLength;
	int minDescriptionLength;

	int maxH1LengthChars;
	int maxH2LengthChars;

	int maxImageAltTextChars;
	int maxImageSizeKb;

	ParserTypeFlags parserTypeFlags;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
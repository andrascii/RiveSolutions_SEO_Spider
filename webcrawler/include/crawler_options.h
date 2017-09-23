#pragma once

namespace WebCrawler
{

struct CrawlerOptions
{
	QUrl url;
	
	int minTitleLength;
	int maxTitleLength;

	int limitMaxUrlLength;

	int maxDescriptionLength;
	int minDescriptionLength;

	int maxH1LengthChars;
	int maxH2LengthChars;

	int maxImageAltTextChars;

	int maxImageSizeKb;
};

Q_DECLARE_METATYPE(CrawlerOptions);

}
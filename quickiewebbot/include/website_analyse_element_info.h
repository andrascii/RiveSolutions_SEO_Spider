#pragma once

#include "website_analyse_element.h"

namespace QuickieWebBot
{

class WebsiteAnalyseElementInfo
{
public:
	enum ElementInfo 
	{ 
		Url,
		Title,
		MetaRefresh,
		MetaRobots,
		RedirectedUrl,
		ServerResponse,
		MetaDescription,
		MetaKeywords,
		FirstH1,
		SecondH1,
		FirstH2,
		SecondH2,
		CanonicalLinkElement,
		StatusCode,
		TitleLength,
		MetaDescriptionLength,
		MetaKeywordsLength,
		FirstH1Length,
		SecondH1Length,
		FirstH2Length,
		SecondH2Length,
		PageSizeBytes,
		WordCount
	};

	static QString title(ElementInfo item);
	static QVariant value(WebSiteAnalyseElementPtr element, ElementInfo item);

private: 
	using GetterInternal = QVariant(*)(WebSiteAnalyseElementPtr);

	static GetterInternal acceptElement(ElementInfo item);
	static QVariant url(WebSiteAnalyseElementPtr element);
	static QVariant title(WebSiteAnalyseElementPtr element);
	static QVariant metaRefresh(WebSiteAnalyseElementPtr element);
	static QVariant metaRobots(WebSiteAnalyseElementPtr element);
	static QVariant redirectedUrl(WebSiteAnalyseElementPtr element);
	static QVariant serverResponse(WebSiteAnalyseElementPtr element);
	static QVariant metaDescription(WebSiteAnalyseElementPtr element);
	static QVariant metaKeywords(WebSiteAnalyseElementPtr element);
	static QVariant firstH1(WebSiteAnalyseElementPtr element);
	static QVariant secondH1(WebSiteAnalyseElementPtr element);
	static QVariant firstH2(WebSiteAnalyseElementPtr element);
	static QVariant secondH2(WebSiteAnalyseElementPtr element);
	static QVariant canonicalLinkElement(WebSiteAnalyseElementPtr element);
	static QVariant statusCode(WebSiteAnalyseElementPtr element);
	static QVariant titleLength(WebSiteAnalyseElementPtr element);
	static QVariant metaDescriptionLength(WebSiteAnalyseElementPtr element);
	static QVariant metaKeywordsLength(WebSiteAnalyseElementPtr element);
	static QVariant firstH1Length(WebSiteAnalyseElementPtr element);
	static QVariant secondH1Length(WebSiteAnalyseElementPtr element);
	static QVariant firstH2Length(WebSiteAnalyseElementPtr element);
	static QVariant secondH2Length(WebSiteAnalyseElementPtr element);
	static QVariant pageSizeBytes(WebSiteAnalyseElementPtr element);
	static QVariant wordCount(WebSiteAnalyseElementPtr element);

	static void checkInfoItem(ElementInfo item);
};

}
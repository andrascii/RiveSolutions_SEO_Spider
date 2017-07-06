#include "website_analyse_element_info.h"

namespace QuickieWebBot
{

QString WebsiteAnalyseElementInfo::title(ElementInfo item)
{
	checkInfoItem(item);

	static std::map<ElementInfo, QString> s_titles
	{
		{ Url, "Url" },
		{ Title, "Title" },
		{ MetaRefresh, "Meta Refresh" },
		{ MetaRobots, "Meta Robots"},
		{ RedirectedUrl, "Redirected URL" },
		{ ServerResponse, "Server Response" },
		{ MetaDescription, "Meta Description" },
		{ MetaKeywords, "Meta Keywords" },
		{ FirstH1, "First H1" },
		{ SecondH1, "Second H1" },
		{ FirstH2, "First H2" },
		{ SecondH2,"Second H2" },
		{ CanonicalLinkElement, "Canonical Link Element" },
		{ StatusCode, "Status Code" },
		{ TitleLength, "Title Length" },
		{ MetaDescriptionLength, "Meta Description Length" },
		{ MetaKeywordsLength, "Meta Keywords Length" },
		{ FirstH1Length, "First H1 Length" },
		{ SecondH1Length, "Second H1 Length" },
		{ FirstH2Length, "First H2 Length" },
		{ SecondH2Length, "Second H2 Length" },
		{ PageSizeBytes, "Page Size Bytes" },
		{ WordCount, "Word Count" }
	};

	return s_titles[item];
}

QVariant WebsiteAnalyseElementInfo::value(WebSiteAnalyseElementPtr element, ElementInfo item)
{
	return acceptElement(item)(element);
}

WebsiteAnalyseElementInfo::GetterInternal WebsiteAnalyseElementInfo::acceptElement(ElementInfo item)
{
	switch (item) 
	{
	case Url:
		return &WebsiteAnalyseElementInfo::url;
	case Title:
		return &WebsiteAnalyseElementInfo::title;
	case MetaRefresh:
		return &WebsiteAnalyseElementInfo::metaRefresh;
	case MetaRobots:
		return &WebsiteAnalyseElementInfo::metaRobots;
	case RedirectedUrl:
		return &WebsiteAnalyseElementInfo::metaRobots;
	case ServerResponse:
		return &WebsiteAnalyseElementInfo::serverResponse;
	case MetaDescription:
		return &WebsiteAnalyseElementInfo::metaDescription;
	case MetaKeywords:
		return &WebsiteAnalyseElementInfo::metaKeywords;
	case FirstH1:
		return &WebsiteAnalyseElementInfo::firstH1;
	case SecondH1:
		return &WebsiteAnalyseElementInfo::secondH1;
	case FirstH2:
		return &WebsiteAnalyseElementInfo::firstH2;
	case SecondH2:
		return &WebsiteAnalyseElementInfo::secondH2;
	case CanonicalLinkElement:
		return &WebsiteAnalyseElementInfo::canonicalLinkElement;
	case StatusCode:
		return &WebsiteAnalyseElementInfo::statusCode;
	case TitleLength:
		return &WebsiteAnalyseElementInfo::titleLength;
	case MetaDescriptionLength:
		return &WebsiteAnalyseElementInfo::metaDescriptionLength;
	case MetaKeywordsLength:
		return &WebsiteAnalyseElementInfo::metaKeywordsLength;
	case FirstH1Length:
		return &WebsiteAnalyseElementInfo::firstH1Length;
	case SecondH1Length:
		return &WebsiteAnalyseElementInfo::secondH1Length;
	case FirstH2Length:
		return &WebsiteAnalyseElementInfo::firstH2Length;
	case SecondH2Length:
		return &WebsiteAnalyseElementInfo::secondH2Length;
	case PageSizeBytes:
		return &WebsiteAnalyseElementInfo::pageSizeBytes;
	case WordCount:
		return &WebsiteAnalyseElementInfo::wordCount;
	}

	assert(!"Unknown element");
	return GetterInternal();
}

QVariant WebsiteAnalyseElementInfo::url(WebSiteAnalyseElementPtr element)
{
	return element->url.toString();
}

QVariant WebsiteAnalyseElementInfo::title(WebSiteAnalyseElementPtr element)
{
	return element->title;
}

QVariant WebsiteAnalyseElementInfo::metaRefresh(WebSiteAnalyseElementPtr element)
{
	return element->metaRefresh;
}

QVariant WebsiteAnalyseElementInfo::metaRobots(WebSiteAnalyseElementPtr element)
{
	return element->metaRobots;
}

QVariant WebsiteAnalyseElementInfo::redirectedUrl(WebSiteAnalyseElementPtr element)
{
	return element->redirectedUrl;
}

QVariant WebsiteAnalyseElementInfo::serverResponse(WebSiteAnalyseElementPtr element)
{
	return element->serverResponse;
}

QVariant WebsiteAnalyseElementInfo::metaDescription(WebSiteAnalyseElementPtr element)
{
	return element->metaDescription;
}

QVariant WebsiteAnalyseElementInfo::metaKeywords(WebSiteAnalyseElementPtr element)
{
	return element->metaKeywords;
}

QVariant WebsiteAnalyseElementInfo::firstH1(WebSiteAnalyseElementPtr element)
{
	return element->firstH1;
}

QVariant WebsiteAnalyseElementInfo::secondH1(WebSiteAnalyseElementPtr element)
{
	return element->secondH1;
}

QVariant WebsiteAnalyseElementInfo::firstH2(WebSiteAnalyseElementPtr element)
{
	return element->firstH2;
}

QVariant WebsiteAnalyseElementInfo::secondH2(WebSiteAnalyseElementPtr element)
{
	return element->secondH2;
}

QVariant WebsiteAnalyseElementInfo::canonicalLinkElement(WebSiteAnalyseElementPtr element)
{
	return element->canonicalLinkElement;
}

QVariant WebsiteAnalyseElementInfo::statusCode(WebSiteAnalyseElementPtr element)
{
	return element->statusCode;
}

QVariant WebsiteAnalyseElementInfo::titleLength(WebSiteAnalyseElementPtr element)
{
	return element->titleLength;
}

QVariant WebsiteAnalyseElementInfo::metaDescriptionLength(WebSiteAnalyseElementPtr element)
{
	return element->metaDescriptionLength;
}

QVariant WebsiteAnalyseElementInfo::metaKeywordsLength(WebSiteAnalyseElementPtr element)
{
	return element->metaKeywordsLength;
}

QVariant WebsiteAnalyseElementInfo::firstH1Length(WebSiteAnalyseElementPtr element)
{
	return element->firstH1Length;
}

QVariant WebsiteAnalyseElementInfo::secondH1Length(WebSiteAnalyseElementPtr element)
{
	return element->secondH1Length;
}

QVariant WebsiteAnalyseElementInfo::firstH2Length(WebSiteAnalyseElementPtr element)
{
	return element->firstH2Length;
}

QVariant WebsiteAnalyseElementInfo::secondH2Length(WebSiteAnalyseElementPtr element)
{
	return element->secondH2Length;
}

QVariant WebsiteAnalyseElementInfo::pageSizeBytes(WebSiteAnalyseElementPtr element)
{
	return element->pageSizeBytes;
}

QVariant WebsiteAnalyseElementInfo::wordCount(WebSiteAnalyseElementPtr element)
{
	return element->wordCount;
}

void WebsiteAnalyseElementInfo::checkInfoItem(ElementInfo item)
{
	assert(item >= Url && item <= WordCount);
}

}
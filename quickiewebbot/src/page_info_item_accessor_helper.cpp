#include "page_info_item_accessor_helper.h"

namespace QuickieWebBot
{

QString PageInfoItemAccessorHelper::title(ItemInfo item)
{
	checkInfoItem(item);

	static std::map<ItemInfo, QString> s_titles
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

QVariant PageInfoItemAccessorHelper::value(PageInfoPtr pageInfo, ItemInfo item)
{
	return acceptItem(item)(pageInfo);
}

PageInfoItemAccessorHelper::GetterInternal PageInfoItemAccessorHelper::acceptItem(ItemInfo item)
{
	switch (item) 
	{
		case Url: return &PageInfoItemAccessorHelper::url;
		case Title: return &PageInfoItemAccessorHelper::title;
		case MetaRefresh: return &PageInfoItemAccessorHelper::metaRefresh;
		case MetaRobots: return &PageInfoItemAccessorHelper::metaRobots;
		case RedirectedUrl: return &PageInfoItemAccessorHelper::metaRobots;
		case ServerResponse: return &PageInfoItemAccessorHelper::serverResponse;
		case MetaDescription: return &PageInfoItemAccessorHelper::metaDescription;
		case MetaKeywords: return &PageInfoItemAccessorHelper::metaKeywords;
		case FirstH1: return &PageInfoItemAccessorHelper::firstH1;
		case SecondH1: return &PageInfoItemAccessorHelper::secondH1;
		case FirstH2: return &PageInfoItemAccessorHelper::firstH2;
		case SecondH2: return &PageInfoItemAccessorHelper::secondH2;
		case CanonicalLinkElement: return &PageInfoItemAccessorHelper::canonicalLinkElement;
		case StatusCode: return &PageInfoItemAccessorHelper::statusCode;
		case TitleLength: return &PageInfoItemAccessorHelper::titleLength;
		case MetaDescriptionLength: return &PageInfoItemAccessorHelper::metaDescriptionLength;
		case MetaKeywordsLength: return &PageInfoItemAccessorHelper::metaKeywordsLength;
		case FirstH1Length: return &PageInfoItemAccessorHelper::firstH1Length;
		case SecondH1Length: return &PageInfoItemAccessorHelper::secondH1Length;
		case FirstH2Length: return &PageInfoItemAccessorHelper::firstH2Length;
		case SecondH2Length: return &PageInfoItemAccessorHelper::secondH2Length;
		case PageSizeBytes: return &PageInfoItemAccessorHelper::pageSizeBytes;
		case WordCount: return &PageInfoItemAccessorHelper::wordCount;
	}

	assert(!"Unknown element");
	return GetterInternal();
}

QVariant PageInfoItemAccessorHelper::url(PageInfoPtr pageInfo)
{
	return pageInfo->url.toString();
}

QVariant PageInfoItemAccessorHelper::title(PageInfoPtr pageInfo)
{
	return pageInfo->title;
}

QVariant PageInfoItemAccessorHelper::metaRefresh(PageInfoPtr pageInfo)
{
	return pageInfo->metaRefresh;
}

QVariant PageInfoItemAccessorHelper::metaRobots(PageInfoPtr pageInfo)
{
	return pageInfo->metaRobots;
}

QVariant PageInfoItemAccessorHelper::redirectedUrl(PageInfoPtr pageInfo)
{
	return pageInfo->redirectedUrl;
}

QVariant PageInfoItemAccessorHelper::serverResponse(PageInfoPtr pageInfo)
{
	return pageInfo->serverResponse;
}

QVariant PageInfoItemAccessorHelper::metaDescription(PageInfoPtr pageInfo)
{
	return pageInfo->metaDescription;
}

QVariant PageInfoItemAccessorHelper::metaKeywords(PageInfoPtr pageInfo)
{
	return pageInfo->metaKeywords;
}

QVariant PageInfoItemAccessorHelper::firstH1(PageInfoPtr pageInfo)
{
	return pageInfo->firstH1;
}

QVariant PageInfoItemAccessorHelper::secondH1(PageInfoPtr pageInfo)
{
	return pageInfo->secondH1;
}

QVariant PageInfoItemAccessorHelper::firstH2(PageInfoPtr pageInfo)
{
	return pageInfo->firstH2;
}

QVariant PageInfoItemAccessorHelper::secondH2(PageInfoPtr pageInfo)
{
	return pageInfo->secondH2;
}

QVariant PageInfoItemAccessorHelper::canonicalLinkElement(PageInfoPtr pageInfo)
{
	return pageInfo->canonicalLinkElement;
}

QVariant PageInfoItemAccessorHelper::statusCode(PageInfoPtr pageInfo)
{
	return pageInfo->statusCode;
}

QVariant PageInfoItemAccessorHelper::titleLength(PageInfoPtr pageInfo)
{
	return pageInfo->title.size();
}

QVariant PageInfoItemAccessorHelper::metaDescriptionLength(PageInfoPtr pageInfo)
{
	return pageInfo->metaDescription.size();
}

QVariant PageInfoItemAccessorHelper::metaKeywordsLength(PageInfoPtr pageInfo)
{
	return pageInfo->metaKeywords.size();
}

QVariant PageInfoItemAccessorHelper::firstH1Length(PageInfoPtr pageInfo)
{
	return pageInfo->firstH1.size();
}

QVariant PageInfoItemAccessorHelper::secondH1Length(PageInfoPtr pageInfo)
{
	return pageInfo->secondH1.size();
}

QVariant PageInfoItemAccessorHelper::firstH2Length(PageInfoPtr pageInfo)
{
	return pageInfo->firstH2.size();
}

QVariant PageInfoItemAccessorHelper::secondH2Length(PageInfoPtr pageInfo)
{
	return pageInfo->secondH2.size();
}

QVariant PageInfoItemAccessorHelper::pageSizeBytes(PageInfoPtr pageInfo)
{
	return pageInfo->pageSizeBytes;
}

QVariant PageInfoItemAccessorHelper::wordCount(PageInfoPtr pageInfo)
{
	return pageInfo->wordCount;
}

void PageInfoItemAccessorHelper::checkInfoItem(ItemInfo item)
{
	assert(item >= Url && item <= WordCount);
}

}
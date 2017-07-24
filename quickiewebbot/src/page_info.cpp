#include "page_info.h"

namespace QuickieWebBot
{

QString PageInfo::itemTitle(ItemType item)
{
	checkInfoItem(item);

	static std::map<ItemType, QString> s_titles
	{
		{ UrlItemType, "Url" },
		{ ContentItemType, "Content" },
		{ TitleItemType, "Title" },
		{ MetaRefreshItemType, "Meta Refresh" },
		{ MetaRobotsItemType, "Meta Robots" },
		{ RedirectedUrlItemType, "Redirected URL" },
		{ ServerResponseItemType, "Server Response" },
		{ MetaDescriptionItemType, "Meta Description" },
		{ MetaKeywordsItemType, "Meta Keywords" },
		{ FirstH1ItemType, "First H1" },
		{ SecondH1ItemType, "Second H1" },
		{ FirstH2ItemType, "First H2" },
		{ SecondH2ItemType,"Second H2" },
		{ CanonicalLinkElementItemType, "Canonical Link Element" },
		{ StatusCodeItemType, "Status Code" },
		{ TitleLengthItemType, "Title Length" },
		{ MetaDescriptionLengthItemType, "Meta Description Length" },
		{ MetaKeywordsLengthItemType, "Meta Keywords Length" },
		{ FirstH1LengthItemType, "First H1 Length" },
		{ SecondH1LengthItemType, "Second H1 Length" },
		{ FirstH2LengthItemType, "First H2 Length" },
		{ SecondH2LengthItemType, "Second H2 Length" },
		{ PageSizeKbItemType, "Page Size Kilobytes" },
		{ WordCountItemType, "Word Count" },
		{ PageHashItemType, "Page Hash" }
	};

	return s_titles[item];
}

QVariant PageInfo::itemValue(const std::shared_ptr<PageInfo>& pageInfo, ItemType item)
{
	return acceptItem(item)(pageInfo);
}

PageInfo::MethodAcceptor PageInfo::acceptItem(ItemType item)
{
	switch (item)
	{
		case UrlItemType: return &PageInfo::acceptUrl;
		case ContentItemType: return &PageInfo::acceptContent;
		case TitleItemType: return &PageInfo::acceptTitle;
		case MetaRefreshItemType: return &PageInfo::acceptMetaRefresh;
		case MetaRobotsItemType: return &PageInfo::acceptMetaRobots;
		case RedirectedUrlItemType: return &PageInfo::acceptMetaRobots;
		case ServerResponseItemType: return &PageInfo::acceptServerResponse;
		case MetaDescriptionItemType: return &PageInfo::acceptMetaDescription;
		case MetaKeywordsItemType: return &PageInfo::acceptMetaKeywords;
		case FirstH1ItemType: return &PageInfo::acceptFirstH1;
		case SecondH1ItemType: return &PageInfo::acceptSecondH1;
		case FirstH2ItemType: return &PageInfo::acceptFirstH2;
		case SecondH2ItemType: return &PageInfo::acceptSecondH2;
		case CanonicalLinkElementItemType: return &PageInfo::acceptCanonicalLinkElement;
		case StatusCodeItemType: return &PageInfo::acceptStatusCode;
		case TitleLengthItemType: return &PageInfo::acceptTitleLength;
		case MetaDescriptionLengthItemType: return &PageInfo::acceptMetaDescriptionLength;
		case MetaKeywordsLengthItemType: return &PageInfo::acceptMetaKeywordsLength;
		case FirstH1LengthItemType: return &PageInfo::acceptFirstH1Length;
		case SecondH1LengthItemType: return &PageInfo::acceptSecondH1Length;
		case FirstH2LengthItemType: return &PageInfo::acceptFirstH2Length;
		case SecondH2LengthItemType: return &PageInfo::acceptSecondH2Length;
		case PageSizeKbItemType: return &PageInfo::acceptPageSizeKb;
		case WordCountItemType: return &PageInfo::acceptWordCount;
		case PageHashItemType: return &PageInfo::acceptPageHash;
	}

	assert(!"Unknown element");
	return MethodAcceptor();
}

QVariant PageInfo::acceptUrl(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->url;
}

QVariant PageInfo::acceptContent(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->content;
}

QVariant PageInfo::acceptTitle(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->title;
}

QVariant PageInfo::acceptMetaRefresh(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaRefresh;
}

QVariant PageInfo::acceptMetaRobots(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaRobots;
}

QVariant PageInfo::acceptRedirectedUrl(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->redirectedUrl;
}

QVariant PageInfo::acceptServerResponse(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->serverResponse;
}

QVariant PageInfo::acceptMetaDescription(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaDescription;
}

QVariant PageInfo::acceptMetaKeywords(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaKeywords;
}

QVariant PageInfo::acceptFirstH1(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->firstH1;
}

QVariant PageInfo::acceptSecondH1(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->secondH1;
}

QVariant PageInfo::acceptFirstH2(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->firstH2;
}

QVariant PageInfo::acceptSecondH2(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->secondH2;
}

QVariant PageInfo::acceptCanonicalLinkElement(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->canonicalLinkElement;
}

QVariant PageInfo::acceptStatusCode(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->statusCode;
}

QVariant PageInfo::acceptTitleLength(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->title.size();
}

QVariant PageInfo::acceptMetaDescriptionLength(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaDescription.size();
}

QVariant PageInfo::acceptMetaKeywordsLength(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->metaKeywords.size();
}

QVariant PageInfo::acceptFirstH1Length(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->firstH1.size();
}

QVariant PageInfo::acceptSecondH1Length(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->secondH1.size();
}

QVariant PageInfo::acceptFirstH2Length(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->firstH2.size();
}

QVariant PageInfo::acceptSecondH2Length(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->secondH2.size();
}

QVariant PageInfo::acceptPageSizeKb(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->pageSizeKb;
}

QVariant PageInfo::acceptWordCount(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->wordCount;
}

QVariant PageInfo::acceptPageHash(const std::shared_ptr<PageInfo>& pageInfo)
{
	return pageInfo->pageHash;
}

void PageInfo::checkInfoItem(ItemType item)
{
	assert(item >= PageInfoItemTypeStart && item <= PageInfoItemTypeLast);
}

}
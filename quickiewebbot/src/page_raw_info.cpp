#include "page_raw.h"
#include "page_raw_info.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

PageRawInfo::PageRawInfo(WebCrawler::PageRawPtr pageRawPtr)
	: m_pageRawPtr(pageRawPtr)
{

}

QString PageRawInfo::itemTypeDescription(Column column)
{
	checkColumnType(column);

	static QMap<Column, QString> s_titles
	{
		{ UrlItemType, QObject::tr("Url") },
		{ FromUrlItemType, QObject::tr("From URL") },
		{ ContentTypeItemType, QObject::tr("Content") },
		{ TitleItemType, QObject::tr("Title") },
		{ MetaRefreshItemType, QObject::tr("Meta Refresh") },
		{ MetaRobotsItemType, QObject::tr("Meta Robots") },
		{ RedirectedUrlItemType, QObject::tr("Redirected URL") },
		{ ServerResponseItemType, QObject::tr("Server Response") },
		{ MetaDescriptionItemType, QObject::tr("Meta Description") },
		{ MetaKeywordsItemType, QObject::tr("Meta Keywords") },
		{ FirstH1ItemType, QObject::tr("First H1") },
		{ SecondH1ItemType, QObject::tr("Second H1") },
		{ FirstH2ItemType, QObject::tr("First H2") },
		{ SecondH2ItemType, QObject::tr("Second H2") },
		{ CanonicalLinkElementItemType, QObject::tr("Canonical Link Element") },
		{ StatusCodeItemType, QObject::tr("Status Code") },
		{ UrlLengthItemType, QObject::tr("Url Length") },
		{ TitleLengthItemType, QObject::tr("Title Length") },
		{ MetaDescriptionLengthItemType, QObject::tr("Meta Description Length") },
		{ MetaKeywordsLengthItemType, QObject::tr("Meta Keywords Length") },
		{ FirstH1LengthItemType, QObject::tr("First H1 Length") },
		{ SecondH1LengthItemType, QObject::tr("Second H1 Length") },
		{ FirstH2LengthItemType, QObject::tr("First H2 Length") },
		{ SecondH2LengthItemType, QObject::tr("Second H2 Length") },
		{ PageSizeKbItemType, QObject::tr("Page Size Kilobytes") },
		{ WordCountItemType, QObject::tr("Word Count") },
		{ PageHashItemType, QObject::tr("Page Hash") },
		{ AltTextItemType, QObject::tr("Alt Text") },
		{ AltTextLengthItemType, QObject::tr("Alt Text Length") },
		{ ImageSizeKbItemType, QObject::tr("Image Size KB") },
		{ NoFollowDoFollowLinkItemType, QObject::tr("Nofollow / Dofollow") },
		{ AltTextLinkItemType, QObject::tr("Alt-Text") }
	};

	return s_titles.value(column, QString::null);
}

int PageRawInfo::columnPrefferedSize(Column column)
{
	static QMap<Column, int> s_prefferedSizes
	{
		{ UrlItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ FromUrlItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ TitleItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ContentTypeItemType, QuickieWebBotHelpers::pointsToPixels(200) },
		{ MetaRefreshItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ MetaRobotsItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ MetaDescriptionItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ MetaKeywordsItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ RedirectedUrlItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ServerResponseItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ FirstH1ItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ SecondH1ItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ FirstH2ItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ SecondH2ItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ CanonicalLinkElementItemType, QuickieWebBotHelpers::pointsToPixels(150) },
		{ StatusCodeItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ PageSizeKbItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ WordCountItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ PageHashItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ TitleLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ UrlLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ MetaDescriptionLengthItemType, QuickieWebBotHelpers::pointsToPixels(110) },
		{ MetaKeywordsLengthItemType, QuickieWebBotHelpers::pointsToPixels(110) },
		{ FirstH1LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ SecondH1LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ FirstH2LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ SecondH2LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ AltTextItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ AltTextLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ImageSizeKbItemType, QuickieWebBotHelpers::pointsToPixels(100) }
	};

	int result = s_prefferedSizes.value(column, -1);
	ASSERT(result >= 0);
	return result;
}

QVariant PageRawInfo::itemValue(Column column) const
{
	return (this->*acceptItem(column))();
}

size_t PageRawInfo::countLinksFromThisPage() const noexcept
{
	return m_pageRawPtr->linksFromThisPage.size();
}

WebCrawler::PageRawWeakPtr PageRawInfo::linkFromThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksFromThisPage());

	return m_pageRawPtr->linksFromThisPage[number];
}

size_t PageRawInfo::countLinksToThisPage() const noexcept
{
	return m_pageRawPtr->linksToThisPage.size();
}

WebCrawler::PageRawWeakPtr PageRawInfo::linkToThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksToThisPage());

	return m_pageRawPtr->linksToThisPage[number];
}

PageRawInfo::MethodAcceptor PageRawInfo::acceptItem(Column column) const
{
	switch (column)
	{
		case UrlItemType: return &PageRawInfo::acceptUrl;
		case FromUrlItemType: return &PageRawInfo::acceptFromUrl;
		case ContentTypeItemType: return &PageRawInfo::acceptContentType;
		case TitleItemType: return &PageRawInfo::acceptTitle;
		case MetaRefreshItemType: return &PageRawInfo::acceptMetaRefresh;
		case MetaRobotsItemType: return &PageRawInfo::acceptMetaRobots;
		case RedirectedUrlItemType: return &PageRawInfo::acceptMetaRobots;
		case ServerResponseItemType: return &PageRawInfo::acceptServerResponse;
		case MetaDescriptionItemType: return &PageRawInfo::acceptMetaDescription;
		case MetaKeywordsItemType: return &PageRawInfo::acceptMetaKeywords;
		case FirstH1ItemType: return &PageRawInfo::acceptFirstH1;
		case SecondH1ItemType: return &PageRawInfo::acceptSecondH1;
		case FirstH2ItemType: return &PageRawInfo::acceptFirstH2;
		case SecondH2ItemType: return &PageRawInfo::acceptSecondH2;
		case CanonicalLinkElementItemType: return &PageRawInfo::acceptCanonicalLinkElement;
		case StatusCodeItemType: return &PageRawInfo::acceptStatusCode;
		case UrlLengthItemType: return &PageRawInfo::acceptUrlLength;
		case TitleLengthItemType: return &PageRawInfo::acceptTitleLength;
		case MetaDescriptionLengthItemType: return &PageRawInfo::acceptMetaDescriptionLength;
		case MetaKeywordsLengthItemType: return &PageRawInfo::acceptMetaKeywordsLength;
		case FirstH1LengthItemType: return &PageRawInfo::acceptFirstH1Length;
		case SecondH1LengthItemType: return &PageRawInfo::acceptSecondH1Length;
		case FirstH2LengthItemType: return &PageRawInfo::acceptFirstH2Length;
		case SecondH2LengthItemType: return &PageRawInfo::acceptSecondH2Length;
		case PageSizeKbItemType: return &PageRawInfo::acceptPageSizeKb;
		case WordCountItemType: return &PageRawInfo::acceptWordCount;
		case PageHashItemType: return &PageRawInfo::acceptPageHash;
		case ImageSizeKbItemType: return &PageRawInfo::acceptImageSizeKb;
		case AltTextItemType: return &PageRawInfo::acceptAltText;
		case AltTextLengthItemType: return &PageRawInfo::acceptAltTextLength;
		case NoFollowDoFollowLinkItemType: return &PageRawInfo::acceptDofollowNofollow;
	}

	ASSERT(!"Unknown element");
	return MethodAcceptor();
}

QVariant PageRawInfo::acceptUrl() const
{
	return m_pageRawPtr->url;
}

QVariant PageRawInfo::acceptFromUrl() const
{
	return m_pageRawPtr->fromUrl;
}

QVariant PageRawInfo::acceptContentType() const
{
	return m_pageRawPtr->contentType;
}

QVariant PageRawInfo::acceptTitle() const
{
	return m_pageRawPtr->title;
}

QVariant PageRawInfo::acceptMetaRefresh() const
{
	return m_pageRawPtr->metaRefresh;
}

QVariant PageRawInfo::acceptMetaRobots() const
{
	return m_pageRawPtr->metaRobots;
}

QVariant PageRawInfo::acceptRedirectedUrl() const
{
	return m_pageRawPtr->redirectedUrl;
}

QVariant PageRawInfo::acceptServerResponse() const
{
	return m_pageRawPtr->serverResponse;
}

QVariant PageRawInfo::acceptMetaDescription() const
{
	return m_pageRawPtr->metaDescription;
}

QVariant PageRawInfo::acceptMetaKeywords() const
{
	return m_pageRawPtr->metaKeywords;
}

QVariant PageRawInfo::acceptFirstH1() const
{
	return m_pageRawPtr->firstH1;
}

QVariant PageRawInfo::acceptSecondH1() const
{
	return m_pageRawPtr->secondH1;
}

QVariant PageRawInfo::acceptFirstH2() const
{
	return m_pageRawPtr->firstH2;
}

QVariant PageRawInfo::acceptSecondH2() const
{
	return m_pageRawPtr->secondH2;
}

QVariant PageRawInfo::acceptCanonicalLinkElement() const
{
	return m_pageRawPtr->canonicalLinkElement;
}

QVariant PageRawInfo::acceptStatusCode() const
{
	return m_pageRawPtr->statusCode;
}

QVariant PageRawInfo::acceptUrlLength() const
{
	return m_pageRawPtr->url.toString().length();
}

QVariant PageRawInfo::acceptTitleLength() const
{
	return m_pageRawPtr->title.size();
}

QVariant PageRawInfo::acceptMetaDescriptionLength() const
{
	return m_pageRawPtr->metaDescription.size();
}

QVariant PageRawInfo::acceptMetaKeywordsLength() const
{
	return m_pageRawPtr->metaKeywords.size();
}

QVariant PageRawInfo::acceptFirstH1Length() const
{
	return m_pageRawPtr->firstH1.size();
}

QVariant PageRawInfo::acceptSecondH1Length() const
{
	return m_pageRawPtr->secondH1.size();
}

QVariant PageRawInfo::acceptFirstH2Length() const
{
	return m_pageRawPtr->firstH2.size();
}

QVariant PageRawInfo::acceptSecondH2Length() const
{
	return m_pageRawPtr->secondH2.size();
}

QVariant PageRawInfo::acceptPageSizeKb() const
{
	return m_pageRawPtr->pageSizeKb;
}

QVariant PageRawInfo::acceptWordCount() const
{
	return m_pageRawPtr->wordCount;
}

QVariant PageRawInfo::acceptPageHash() const
{
	return m_pageRawPtr->pageHash;
}

QVariant PageRawInfo::acceptImageSizeKb() const
{
	return m_pageRawPtr->pageSizeKb;
}

QVariant PageRawInfo::acceptAltText() const
{
	return m_pageRawPtr->altText;
}

QVariant PageRawInfo::acceptDofollowNofollow() const
{
	switch (m_pageRawPtr->linkParameter)
	{
		case WebCrawler::PageRaw::DofollowParameter:
		{
			return QObject::tr("Dofollow");
		}
		case WebCrawler::PageRaw::NofollowParameter:
		{
			return QObject::tr("Nofollow");
		}
	}

	return QObject::tr("Unknown link parameter");
}

QVariant PageRawInfo::acceptAltTextLength() const
{
	return m_pageRawPtr->altText.size();
}

void PageRawInfo::checkColumnType(Column column)
{
	ASSERT(column > BeginEnumPageInfoItemType && column < EndEnumPageInfoItemType);
}

}
#include "parsed_page.h"
#include "parsed_page_info.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

ParsedPageInfo::ParsedPageInfo(WebCrawler::ParsedPagePtr pageRawPtr)
	: m_pageRawPtr(pageRawPtr)
{

}

QString ParsedPageInfo::itemTypeDescription(Column column)
{
	checkColumnType(column);

	static QMap<Column, QString> s_titles
	{
		{ UrlItemType, QObject::tr("Url") },
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

int ParsedPageInfo::columnPrefferedSize(Column column)
{
	static QMap<Column, int> s_prefferedSizes
	{
		{ UrlItemType, QuickieWebBotHelpers::pointsToPixels(300) },
		{ TitleItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ContentTypeItemType, QuickieWebBotHelpers::pointsToPixels(150) },
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
		{ ImageSizeKbItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ NoFollowDoFollowLinkItemType, QuickieWebBotHelpers::pointsToPixels(150) }
	};

	int result = s_prefferedSizes.value(column, -1);
	ASSERT(result >= 0);
	return result;
}

QVariant ParsedPageInfo::itemValue(Column column) const
{
	if (!m_pageRawPtr)
	{
		return QVariant();
	}

	return (this->*acceptItem(column))();
}

size_t ParsedPageInfo::countLinksFromThisPage() const noexcept
{
	if (!m_pageRawPtr)
	{
		return 0;
	}

	return m_pageRawPtr->linksOnThisPage.size();
}

WebCrawler::ParsedPageWeakPtr ParsedPageInfo::linkFromThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksFromThisPage());

	return m_pageRawPtr->linksOnThisPage[number].resource;
}

size_t ParsedPageInfo::countLinksToThisPage() const noexcept
{
	if (!m_pageRawPtr)
	{
		return 0;
	}

	return m_pageRawPtr->linksToThisPage.size();
}

WebCrawler::ParsedPageWeakPtr ParsedPageInfo::linkToThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksToThisPage());

	return m_pageRawPtr->linksToThisPage[number].resource;
}

ParsedPageInfo::MethodAcceptor ParsedPageInfo::acceptItem(Column column) const
{
	switch (column)
	{
		case UrlItemType: return &ParsedPageInfo::acceptUrl;
		case ContentTypeItemType: return &ParsedPageInfo::acceptContentType;
		case TitleItemType: return &ParsedPageInfo::acceptTitle;
		case MetaRefreshItemType: return &ParsedPageInfo::acceptMetaRefresh;
		case MetaRobotsItemType: return &ParsedPageInfo::acceptMetaRobots;
		case RedirectedUrlItemType: return &ParsedPageInfo::acceptMetaRobots;
		case ServerResponseItemType: return &ParsedPageInfo::acceptServerResponse;
		case MetaDescriptionItemType: return &ParsedPageInfo::acceptMetaDescription;
		case MetaKeywordsItemType: return &ParsedPageInfo::acceptMetaKeywords;
		case FirstH1ItemType: return &ParsedPageInfo::acceptFirstH1;
		case SecondH1ItemType: return &ParsedPageInfo::acceptSecondH1;
		case FirstH2ItemType: return &ParsedPageInfo::acceptFirstH2;
		case SecondH2ItemType: return &ParsedPageInfo::acceptSecondH2;
		case CanonicalLinkElementItemType: return &ParsedPageInfo::acceptCanonicalLinkElement;
		case StatusCodeItemType: return &ParsedPageInfo::acceptStatusCode;
		case UrlLengthItemType: return &ParsedPageInfo::acceptUrlLength;
		case TitleLengthItemType: return &ParsedPageInfo::acceptTitleLength;
		case MetaDescriptionLengthItemType: return &ParsedPageInfo::acceptMetaDescriptionLength;
		case MetaKeywordsLengthItemType: return &ParsedPageInfo::acceptMetaKeywordsLength;
		case FirstH1LengthItemType: return &ParsedPageInfo::acceptFirstH1Length;
		case SecondH1LengthItemType: return &ParsedPageInfo::acceptSecondH1Length;
		case FirstH2LengthItemType: return &ParsedPageInfo::acceptFirstH2Length;
		case SecondH2LengthItemType: return &ParsedPageInfo::acceptSecondH2Length;
		case PageSizeKbItemType: return &ParsedPageInfo::acceptPageSizeKb;
		case WordCountItemType: return &ParsedPageInfo::acceptWordCount;
		case PageHashItemType: return &ParsedPageInfo::acceptPageHash;
		case ImageSizeKbItemType: return &ParsedPageInfo::acceptImageSizeKb;
		case AltTextItemType: return &ParsedPageInfo::acceptAltText;
		case AltTextLengthItemType: return &ParsedPageInfo::acceptAltTextLength;
		case NoFollowDoFollowLinkItemType: return &ParsedPageInfo::acceptDofollowNofollow;
	}

	ASSERT(!"Unknown element");
	return MethodAcceptor();
}

QVariant ParsedPageInfo::acceptUrl() const
{
	return m_pageRawPtr->url;
}

QVariant ParsedPageInfo::acceptContentType() const
{
	return m_pageRawPtr->contentType;
}

QVariant ParsedPageInfo::acceptTitle() const
{
	return m_pageRawPtr->title;
}

QVariant ParsedPageInfo::acceptMetaRefresh() const
{
	return m_pageRawPtr->metaRefresh;
}

QVariant ParsedPageInfo::acceptMetaRobots() const
{
	return m_pageRawPtr->metaRobots;
}

QVariant ParsedPageInfo::acceptRedirectedUrl() const
{
	return m_pageRawPtr->redirectedUrl;
}

QVariant ParsedPageInfo::acceptServerResponse() const
{
	return m_pageRawPtr->serverResponse;
}

QVariant ParsedPageInfo::acceptMetaDescription() const
{
	return m_pageRawPtr->metaDescription;
}

QVariant ParsedPageInfo::acceptMetaKeywords() const
{
	return m_pageRawPtr->metaKeywords;
}

QVariant ParsedPageInfo::acceptFirstH1() const
{
	return m_pageRawPtr->firstH1;
}

QVariant ParsedPageInfo::acceptSecondH1() const
{
	return m_pageRawPtr->secondH1;
}

QVariant ParsedPageInfo::acceptFirstH2() const
{
	return m_pageRawPtr->firstH2;
}

QVariant ParsedPageInfo::acceptSecondH2() const
{
	return m_pageRawPtr->secondH2;
}

QVariant ParsedPageInfo::acceptCanonicalLinkElement() const
{
	return m_pageRawPtr->canonicalLinkElement;
}

QVariant ParsedPageInfo::acceptStatusCode() const
{
	return m_pageRawPtr->statusCode;
}

QVariant ParsedPageInfo::acceptUrlLength() const
{
	return m_pageRawPtr->url.toString().length();
}

QVariant ParsedPageInfo::acceptTitleLength() const
{
	return m_pageRawPtr->title.size();
}

QVariant ParsedPageInfo::acceptMetaDescriptionLength() const
{
	return m_pageRawPtr->metaDescription.size();
}

QVariant ParsedPageInfo::acceptMetaKeywordsLength() const
{
	return m_pageRawPtr->metaKeywords.size();
}

QVariant ParsedPageInfo::acceptFirstH1Length() const
{
	return m_pageRawPtr->firstH1.size();
}

QVariant ParsedPageInfo::acceptSecondH1Length() const
{
	return m_pageRawPtr->secondH1.size();
}

QVariant ParsedPageInfo::acceptFirstH2Length() const
{
	return m_pageRawPtr->firstH2.size();
}

QVariant ParsedPageInfo::acceptSecondH2Length() const
{
	return m_pageRawPtr->secondH2.size();
}

QVariant ParsedPageInfo::acceptPageSizeKb() const
{
	return m_pageRawPtr->pageSizeKilobytes;
}

QVariant ParsedPageInfo::acceptWordCount() const
{
	return m_pageRawPtr->wordCount;
}

QVariant ParsedPageInfo::acceptPageHash() const
{
	return m_pageRawPtr->pageHash;
}

QVariant ParsedPageInfo::acceptImageSizeKb() const
{
	return m_pageRawPtr->pageSizeKilobytes;
}

QVariant ParsedPageInfo::acceptAltText() const
{
	// !! wrong realization
	if (!m_pageRawPtr->tooLongAltIndices.empty())
	{
		const size_t index = *m_pageRawPtr->tooLongAltIndices.begin();
		return m_pageRawPtr->linksToThisPage[index].altOrTitle;
	}

	// or iterate through all resources and find first Image resource
	// also should be dependent on StorageType (i.e. should return first too long alt text for TooLongAltImageStorageType)

	return QString();
}

QVariant ParsedPageInfo::acceptDofollowNofollow() const
{
	switch (m_pageRawPtr->linkParameter)
	{
		case WebCrawler::LinkParameter::DofollowParameter:
		{
			return QObject::tr("Dofollow");
		}
		case WebCrawler::LinkParameter::NofollowParameter:
		{
			return QObject::tr("Nofollow");
		}
	}

	return QObject::tr("Unknown link parameter");
}

QVariant ParsedPageInfo::acceptAltTextLength() const
{
	return acceptAltText().toString().size();
}

void ParsedPageInfo::checkColumnType(Column column)
{
	ASSERT(column > BeginEnumPageInfoItemType && column < EndEnumPageInfoItemType);
}

}
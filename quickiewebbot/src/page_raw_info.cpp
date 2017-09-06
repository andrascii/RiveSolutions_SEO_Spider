#include "page_raw_info.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

PageRawInfo::PageRawInfo(WebCrawler::PageRawPtr pageRawPtr)
	: m_pageRawPtr(pageRawPtr)
{

}

QString PageRawInfo::itemTypeDescription(ItemType item)
{
	checkInfoItem(item);

	static QMap<ItemType, QString> s_titles
	{
		{ UrlItemType, "Url" },
		{ FromUrlItemType, "From URL"},
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
		{ UrlLengthItemType, "Url Length" },
		{ TitleLengthItemType, "Title Length" },
		{ MetaDescriptionLengthItemType, "Meta Description Length" },
		{ MetaKeywordsLengthItemType, "Meta Keywords Length" },
		{ FirstH1LengthItemType, "First H1 Length" },
		{ SecondH1LengthItemType, "Second H1 Length" },
		{ FirstH2LengthItemType, "First H2 Length" },
		{ SecondH2LengthItemType, "Second H2 Length" },
		{ PageSizeKbItemType, "Page Size Kilobytes" },
		{ WordCountItemType, "Word Count" },
		{ PageHashItemType, "Page Hash" },
		{ ImageAltTextItemType, "Alt Text" },
		{ ImageAltTextLengthItemType, "Alt Text Length" },
		{ ImageSizeKbItemType, "Image Size KB" }
	};

	return s_titles.value(item, QString::null);
}

int PageRawInfo::columnPrefferedSize(ItemType item)
{
	static QMap<ItemType, int> s_prefferedSizes
	{
		{ UrlItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ FromUrlItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ TitleItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ContentItemType, QuickieWebBotHelpers::pointsToPixels(200) },
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
		{ MetaDescriptionLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ MetaKeywordsLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ FirstH1LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ SecondH1LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ FirstH2LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ SecondH2LengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ImageAltTextItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ImageAltTextLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ImageSizeKbItemType, QuickieWebBotHelpers::pointsToPixels(100) }
	};

	int result = s_prefferedSizes.value(item, -1);
	ASSERT(result >= 0);
	return result;
}

QVariant PageRawInfo::itemValue(ItemType item)
{
	return (this->*acceptItem(item))();
}

void PageRawInfo::setItemValue(const QVariant& value, ItemType item)
{
	ASSERT(!(item >= TitleLengthItemType && item <= SecondH2LengthItemType) &&
		"Length will be set automatically! It should not be set directly!");

	using setItemPtr = void(PageRawInfo::*)(const QVariant& value);

	setItemPtr setItemFunction = nullptr;

	switch (item)
	{
		case UrlItemType:
			setItemFunction = &PageRawInfo::setUrl;
			break;
		case FromUrlItemType:
			setItemFunction = &PageRawInfo::setFromUrl;
			break;
		case ContentItemType:
			setItemFunction = &PageRawInfo::setContent;
			break;
		case TitleItemType:
			setItemFunction = &PageRawInfo::setTitle;
			break;
		case MetaRefreshItemType:
			setItemFunction = &PageRawInfo::setMetaRefresh;
			break;
		case MetaRobotsItemType:
			setItemFunction = &PageRawInfo::setMetaRobots;
			break;
		case RedirectedUrlItemType:
			setItemFunction = &PageRawInfo::setRedirectedUrl;
			break;
		case ServerResponseItemType:
			setItemFunction = &PageRawInfo::setServerResponse;
			break;
		case MetaDescriptionItemType:
			setItemFunction = &PageRawInfo::setMetaDescription;
			break;
		case MetaKeywordsItemType:
			setItemFunction = &PageRawInfo::setMetaKeywords;
			break;
		case FirstH1ItemType:
			setItemFunction = &PageRawInfo::setFirstH1;
			break;
		case SecondH1ItemType:
			setItemFunction = &PageRawInfo::setSecondH1;
			break;
		case FirstH2ItemType:
			setItemFunction = &PageRawInfo::setFirstH2;
			break;
		case SecondH2ItemType:
			setItemFunction = &PageRawInfo::setSecondH2;
			break;
		case CanonicalLinkElementItemType:
			setItemFunction = &PageRawInfo::setCanonicalLinkElement;
			break;
		case StatusCodeItemType:
			setItemFunction = &PageRawInfo::setStatusCode;
			break;
		case PageSizeKbItemType:
			setItemFunction = &PageRawInfo::setPageSizeKb;
			break;
		case WordCountItemType:
			setItemFunction = &PageRawInfo::setWordCount;
			break;
		case PageHashItemType:
			setItemFunction = &PageRawInfo::setPageHash;
			break;
	}

	ASSERT(setItemFunction && "Cannot find setter function for this item");

	(this->*setItemFunction)(value);
}


size_t PageRawInfo::countLinksFromThisPage() const noexcept
{
	return m_pageRawPtr->linksFromThisPage.size();
}

WebCrawler::PageRaw::PageRawWeakPtr PageRawInfo::linkFromThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksFromThisPage());

	return m_pageRawPtr->linksFromThisPage[number];
}

size_t PageRawInfo::countLinksToThisPage() const noexcept
{
	return m_pageRawPtr->linksToThisPage.size();
}

WebCrawler::PageRaw::PageRawWeakPtr PageRawInfo::linkToThisPage(size_t number)
{
	DEBUG_ASSERT(number < countLinksToThisPage());

	return m_pageRawPtr->linksToThisPage[number];
}

void PageRawInfo::setUrl(const QVariant& value)
{
	ASSERT(value.type() == QVariant::Url && "Passed type must be QUrl!");

	m_pageRawPtr->url = value.toUrl();
}

void PageRawInfo::setFromUrl(const QVariant& value)
{
	ASSERT(value.type() == QVariant::Url && "Passed type must be QUrl!");

	m_pageRawPtr->fromUrl = value.toUrl();
}

void PageRawInfo::setContent(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->contentType = value.toString();
}

void PageRawInfo::setTitle(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->title = value.toString();
}

void PageRawInfo::setMetaRefresh(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->metaRefresh = value.toString();
}

void PageRawInfo::setMetaRobots(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->metaRobots = value.toString();
}

void PageRawInfo::setRedirectedUrl(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->redirectedUrl = value.toString();
}

void PageRawInfo::setServerResponse(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->serverResponse = value.toString();
}

void PageRawInfo::setMetaDescription(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->metaDescription = value.toString();
}

void PageRawInfo::setMetaKeywords(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->metaKeywords = value.toString();
}

void PageRawInfo::setFirstH1(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->firstH1 = value.toString();
}

void PageRawInfo::setSecondH1(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->secondH2 = value.toString();
}

void PageRawInfo::setFirstH2(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->firstH2 = value.toString();
}

void PageRawInfo::setSecondH2(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->secondH2 = value.toString();
}

void PageRawInfo::setCanonicalLinkElement(const QVariant& value)
{
	ASSERT(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_pageRawPtr->canonicalLinkElement = value.toString();
}

void PageRawInfo::setStatusCode(const QVariant& value)
{
	ASSERT(value.type() == QVariant::Int && "Passed type must be int!");

	m_pageRawPtr->statusCode = value.toInt();
}

void PageRawInfo::setPageSizeKb(const QVariant& value)
{
	ASSERT(value.type() == QVariant::Int && "Passed type must be int!");

	m_pageRawPtr->pageSizeKb = value.toInt();
}

void PageRawInfo::setWordCount(const QVariant& value)
{
	ASSERT(value.type() == QVariant::Int && "Passed type must be int!");

	m_pageRawPtr->wordCount = value.toInt();
}

void PageRawInfo::setPageHash(const QVariant& value)
{
	//
	// Here should be the some unsigned number
	//
	ASSERT(value.type() == QVariant::ULongLong ||
		value.type() == QVariant::UInt);

	m_pageRawPtr->pageHash = value.value<size_t>();
}

PageRawInfo::MethodAcceptor PageRawInfo::acceptItem(ItemType item)
{
	switch (item)
	{
		case UrlItemType: return &PageRawInfo::acceptUrl;
		case FromUrlItemType: return &PageRawInfo::acceptFromUrl;
		case ContentItemType: return &PageRawInfo::acceptContent;
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
		case ImageAltTextItemType: return &PageRawInfo::acceptImageAltText;
		case ImageAltTextLengthItemType: return &PageRawInfo::acceptImageAltTextLength;
	}

	ASSERT(!"Unknown element");
	return MethodAcceptor();
}

QVariant PageRawInfo::acceptUrl()
{
	return m_pageRawPtr->url;
}

QVariant PageRawInfo::acceptFromUrl()
{
	return m_pageRawPtr->fromUrl;
}

QVariant PageRawInfo::acceptContent()
{
	return m_pageRawPtr->contentType;
}

QVariant PageRawInfo::acceptTitle()
{
	return m_pageRawPtr->title;
}

QVariant PageRawInfo::acceptMetaRefresh()
{
	return m_pageRawPtr->metaRefresh;
}

QVariant PageRawInfo::acceptMetaRobots()
{
	return m_pageRawPtr->metaRobots;
}

QVariant PageRawInfo::acceptRedirectedUrl()
{
	return m_pageRawPtr->redirectedUrl;
}

QVariant PageRawInfo::acceptServerResponse()
{
	return m_pageRawPtr->serverResponse;
}

QVariant PageRawInfo::acceptMetaDescription()
{
	return m_pageRawPtr->metaDescription;
}

QVariant PageRawInfo::acceptMetaKeywords()
{
	return m_pageRawPtr->metaKeywords;
}

QVariant PageRawInfo::acceptFirstH1()
{
	return m_pageRawPtr->firstH1;
}

QVariant PageRawInfo::acceptSecondH1()
{
	return m_pageRawPtr->secondH1;
}

QVariant PageRawInfo::acceptFirstH2()
{
	return m_pageRawPtr->firstH2;
}

QVariant PageRawInfo::acceptSecondH2()
{
	return m_pageRawPtr->secondH2;
}

QVariant PageRawInfo::acceptCanonicalLinkElement()
{
	return m_pageRawPtr->canonicalLinkElement;
}

QVariant PageRawInfo::acceptStatusCode()
{
	return m_pageRawPtr->statusCode;
}

QVariant PageRawInfo::acceptUrlLength()
{
	return m_pageRawPtr->url.toString().length();
}

QVariant PageRawInfo::acceptTitleLength()
{
	return m_pageRawPtr->title.size();
}

QVariant PageRawInfo::acceptMetaDescriptionLength()
{
	return m_pageRawPtr->metaDescription.size();
}

QVariant PageRawInfo::acceptMetaKeywordsLength()
{
	return m_pageRawPtr->metaKeywords.size();
}

QVariant PageRawInfo::acceptFirstH1Length()
{
	return m_pageRawPtr->firstH1.size();
}

QVariant PageRawInfo::acceptSecondH1Length()
{
	return m_pageRawPtr->secondH1.size();
}

QVariant PageRawInfo::acceptFirstH2Length()
{
	return m_pageRawPtr->firstH2.size();
}

QVariant PageRawInfo::acceptSecondH2Length()
{
	return m_pageRawPtr->secondH2.size();
}

QVariant PageRawInfo::acceptPageSizeKb()
{
	return m_pageRawPtr->pageSizeKb;
}

QVariant PageRawInfo::acceptWordCount()
{
	return m_pageRawPtr->wordCount;
}

QVariant PageRawInfo::acceptPageHash()
{
	return m_pageRawPtr->pageHash;
}

QVariant PageRawInfo::acceptImageSizeKb()
{
	return m_pageRawPtr->pageSizeKb;
}

QVariant PageRawInfo::acceptImageAltText()
{
	return m_pageRawPtr->imageAltText;
}

QVariant PageRawInfo::acceptImageAltTextLength()
{
	return m_pageRawPtr->imageAltText.size();
}

void PageRawInfo::checkInfoItem(ItemType item)
{
	ASSERT(item > BeginEnumPageInfoItemType && item < EndEnumPageInfoItemType);
}

}
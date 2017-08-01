#include "page_info.h"
#include "quickie_web_bot_helpers.h"

namespace QuickieWebBot
{

QString PageInfo::itemTypeDescription(ItemType item)
{
	checkInfoItem(item);

	static QMap<ItemType, QString> s_titles
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
		{ AltTextItemType, "Alt Text" },
		{ AltTextLengthItemType, "Alt Text Length" },
		{ ImageSizeKbItemType, "Image Size KB" }
	};

	return s_titles.value(item, QString::null);
}

int PageInfo::columnPrefferedSize(ItemType item)
{
	static QMap<ItemType, double> s_prefferedSizes
	{
		{ UrlItemType, QuickieWebBotHelpers::pointsToPixels(400) },
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
		{ CanonicalLinkElementItemType, QuickieWebBotHelpers::pointsToPixels(100) },
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
		{ AltTextItemType, QuickieWebBotHelpers::pointsToPixels(400) },
		{ AltTextLengthItemType, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ImageSizeKbItemType, QuickieWebBotHelpers::pointsToPixels(100) }
	};

	int result = s_prefferedSizes.value(item, -1);
	assert(result >= 0);
	return result;
}

QVariant PageInfo::itemValue(ItemType item)
{
	return (this->*acceptItem(item))();
}

void PageInfo::setItemValue(const QVariant& value, ItemType item)
{
	assert(!(item >= TitleLengthItemType && item <= SecondH2LengthItemType) &&
		"Length will be set automatically! It should not be set directly!");

	using setItemPtr = void(PageInfo::*)(const QVariant& value);

	setItemPtr setItemFunction = nullptr;

	switch (item)
	{
		case UrlItemType:
		{
			setItemFunction = &PageInfo::setUrl;
			break;
		}
		case ContentItemType:
		{
			setItemFunction = &PageInfo::setContent;
			break;
		}
		case TitleItemType:
		{
			setItemFunction = &PageInfo::setTitle;
			break;
		}
		case MetaRefreshItemType:
		{
			setItemFunction = &PageInfo::setMetaRefresh;
			break;
		}
		case MetaRobotsItemType:
		{
			setItemFunction = &PageInfo::setMetaRobots;
			break;
		}
		case RedirectedUrlItemType:
		{
			setItemFunction = &PageInfo::setRedirectedUrl;
			break;
		}
		case ServerResponseItemType:
		{
			setItemFunction = &PageInfo::setServerResponse;
			break;
		}
		case MetaDescriptionItemType:
		{
			setItemFunction = &PageInfo::setMetaDescription;
			break;
		}
		case MetaKeywordsItemType:
		{
			setItemFunction = &PageInfo::setMetaKeywords;
			break;
		}
		case FirstH1ItemType:
		{
			setItemFunction = &PageInfo::setFirstH1;
			break;
		}
		case SecondH1ItemType:
		{
			setItemFunction = &PageInfo::setSecondH1;
			break;
		}
		case FirstH2ItemType:
		{
			setItemFunction = &PageInfo::setFirstH2;
			break;
		}
		case SecondH2ItemType:
		{
			setItemFunction = &PageInfo::setSecondH2;
			break;
		}
		case CanonicalLinkElementItemType:
		{
			setItemFunction = &PageInfo::setCanonicalLinkElement;
			break;
		}
		case StatusCodeItemType:
		{
			setItemFunction = &PageInfo::setStatusCode;
			break;
		}
		case PageSizeKbItemType:
		{
			setItemFunction = &PageInfo::setPageSizeKb;
			break;
		}
		case WordCountItemType:
		{
			setItemFunction = &PageInfo::setWordCount;
			break;
		}
		case PageHashItemType:
		{
			setItemFunction = &PageInfo::setPageHash;
			break;
		}
	}

	assert(setItemFunction && "Cannot find setter function for this item");

	(this->*setItemFunction)(value);
}

void PageInfo::setUrl(const QVariant& value)
{
	assert(value.type() == QVariant::Url && "Passed type must be QUrl!");

	m_url = value.toUrl();
}

void PageInfo::setContent(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_content = value.toString();
}

void PageInfo::setTitle(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_title = value.toString();
}

void PageInfo::setMetaRefresh(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_metaRefresh = value.toString();
}

void PageInfo::setMetaRobots(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_metaRobots = value.toString();
}

void PageInfo::setRedirectedUrl(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_redirectedUrl = value.toString();
}

void PageInfo::setServerResponse(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_serverResponse = value.toString();
}

void PageInfo::setMetaDescription(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_metaDescription = value.toString();
}

void PageInfo::setMetaKeywords(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_metaKeywords = value.toString();
}

void PageInfo::setFirstH1(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_firstH1 = value.toString();
}

void PageInfo::setSecondH1(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_secondH2 = value.toString();
}

void PageInfo::setFirstH2(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_firstH2 = value.toString();
}

void PageInfo::setSecondH2(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_secondH2 = value.toString();
}

void PageInfo::setCanonicalLinkElement(const QVariant& value)
{
	assert(value.type() == QVariant::String || value.type() == QVariant::ByteArray);

	m_canonicalLinkElement = value.toString();
}

void PageInfo::setStatusCode(const QVariant& value)
{
	assert(value.type() == QVariant::Int && "Passed type must be int!");

	m_statusCode = value.toInt();
}

void PageInfo::setPageSizeKb(const QVariant& value)
{
	assert(value.type() == QVariant::Int && "Passed type must be int!");

	m_pageSizeKb = value.toInt();
}

void PageInfo::setWordCount(const QVariant& value)
{
	assert(value.type() == QVariant::Int && "Passed type must be int!");

	m_wordCount = value.toInt();
}

void PageInfo::setPageHash(const QVariant& value)
{
	//
	// Here should be the some unsigned number
	//
	assert(value.type() == QVariant::ULongLong ||
		value.type() == QVariant::UInt);

	m_pageHash = value.value<size_t>();
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
		case UrlLengthItemType: return &PageInfo::acceptUrlLength;
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

QVariant PageInfo::acceptUrl()
{
	return m_url;
}

QVariant PageInfo::acceptContent()
{
	return m_content;
}

QVariant PageInfo::acceptTitle()
{
	return m_title;
}

QVariant PageInfo::acceptMetaRefresh()
{
	return m_metaRefresh;
}

QVariant PageInfo::acceptMetaRobots()
{
	return m_metaRobots;
}

QVariant PageInfo::acceptRedirectedUrl()
{
	return m_redirectedUrl;
}

QVariant PageInfo::acceptServerResponse()
{
	return m_serverResponse;
}

QVariant PageInfo::acceptMetaDescription()
{
	return m_metaDescription;
}

QVariant PageInfo::acceptMetaKeywords()
{
	return m_metaKeywords;
}

QVariant PageInfo::acceptFirstH1()
{
	return m_firstH1;
}

QVariant PageInfo::acceptSecondH1()
{
	return m_secondH1;
}

QVariant PageInfo::acceptFirstH2()
{
	return m_firstH2;
}

QVariant PageInfo::acceptSecondH2()
{
	return m_secondH2;
}

QVariant PageInfo::acceptCanonicalLinkElement()
{
	return m_canonicalLinkElement;
}

QVariant PageInfo::acceptStatusCode()
{
	return m_statusCode;
}

QVariant PageInfo::acceptUrlLength()
{
	return m_url.toString().length();
}

QVariant PageInfo::acceptTitleLength()
{
	return m_title.size();
}

QVariant PageInfo::acceptMetaDescriptionLength()
{
	return m_metaDescription.size();
}

QVariant PageInfo::acceptMetaKeywordsLength()
{
	return m_metaKeywords.size();
}

QVariant PageInfo::acceptFirstH1Length()
{
	return m_firstH1.size();
}

QVariant PageInfo::acceptSecondH1Length()
{
	return m_secondH1.size();
}

QVariant PageInfo::acceptFirstH2Length()
{
	return m_firstH2.size();
}

QVariant PageInfo::acceptSecondH2Length()
{
	return m_secondH2.size();
}

QVariant PageInfo::acceptPageSizeKb()
{
	return m_pageSizeKb;
}

QVariant PageInfo::acceptWordCount()
{
	return m_wordCount;
}

QVariant PageInfo::acceptPageHash()
{
	return m_pageHash;
}

void PageInfo::checkInfoItem(ItemType item)
{
	assert(item >= PageInfoItemTypeStart && item <= PageInfoItemTypeLast);
}

}
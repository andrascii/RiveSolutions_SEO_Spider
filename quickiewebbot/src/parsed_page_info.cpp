#include "parsed_page.h"
#include "parsed_page_info.h"
#include "quickie_web_bot_helpers.h"
#include "status_code_description.h"

namespace QuickieWebBot
{

ParsedPageInfo::ParsedPageInfo(WebCrawler::ParsedPagePtr parsedPage)
	: m_parsedPage(parsedPage)
{
}

QString ParsedPageInfo::itemTypeDescription(Column column)
{
	static QMap<ParsedPageInfo::Column, QString> s_parsedPageColumns =
	{
		{ ParsedPageInfo::Column::UrlColumn, QObject::tr("Url") },
		{ ParsedPageInfo::Column::ContentTypeColumn, QObject::tr("Content") },
		{ ParsedPageInfo::Column::TitleColumn, QObject::tr("Title") },
		{ ParsedPageInfo::Column::MetaRefreshColumn, QObject::tr("Meta Refresh") },
		{ ParsedPageInfo::Column::MetaRobotsColumn, QObject::tr("Meta Robots") },
		{ ParsedPageInfo::Column::RedirectedUrlColumn, QObject::tr("Redirected URL") },
		{ ParsedPageInfo::Column::ServerResponseColumn, QObject::tr("Server Response") },
		{ ParsedPageInfo::Column::MetaDescriptionColumn, QObject::tr("Meta Description") },
		{ ParsedPageInfo::Column::MetaKeywordsColumn, QObject::tr("Meta Keywords") },
		{ ParsedPageInfo::Column::FirstH1Column, QObject::tr("First H1") },
		{ ParsedPageInfo::Column::SecondH1Column, QObject::tr("Second H1") },
		{ ParsedPageInfo::Column::FirstH2Column, QObject::tr("First H2") },
		{ ParsedPageInfo::Column::SecondH2Column, QObject::tr("Second H2") },
		{ ParsedPageInfo::Column::CanonicalLinkElementColumn, QObject::tr("Canonical Link Element") },
		{ ParsedPageInfo::Column::StatusCodeColumn, QObject::tr("Status Code") },
		{ ParsedPageInfo::Column::UrlLengthColumn, QObject::tr("Url Length") },
		{ ParsedPageInfo::Column::TitleLengthColumn, QObject::tr("Title Length") },
		{ ParsedPageInfo::Column::MetaDescriptionLengthColumn, QObject::tr("Meta Description Length") },
		{ ParsedPageInfo::Column::MetaKeywordsLengthColumn, QObject::tr("Meta Keywords Length") },
		{ ParsedPageInfo::Column::FirstH1LengthColumn, QObject::tr("First H1 Length") },
		{ ParsedPageInfo::Column::SecondH1LengthColumn, QObject::tr("Second H1 Length") },
		{ ParsedPageInfo::Column::FirstH2LengthColumn, QObject::tr("First H2 Length") },
		{ ParsedPageInfo::Column::SecondH2LengthColumn, QObject::tr("Second H2 Length") },
		{ ParsedPageInfo::Column::PageSizeKbColumn, QObject::tr("Page Size Kilobytes") },
		{ ParsedPageInfo::Column::WordCountColumn, QObject::tr("Word Count") },
		{ ParsedPageInfo::Column::PageHashColumn, QObject::tr("Page Hash") },
		{ ParsedPageInfo::Column::ImageSizeKbColumn, QObject::tr("Image Size KB") },
	};

	checkColumnType(column);

	return s_parsedPageColumns.value(column, QString::null);
}

QString ParsedPageInfo::itemTypeDescription(PageLinksColumn column)
{
	static QMap<ParsedPageInfo::PageLinksColumn, QString> s_pageLinksColumns =
	{
		{ ParsedPageInfo::PageLinksColumn::UrlColumn, QObject::tr("Url") },
		{ ParsedPageInfo::PageLinksColumn::StatusCodeColumn, QObject::tr("Status Code") },
		{ ParsedPageInfo::PageLinksColumn::AltOrTitleColumn, QObject::tr("Alt Text") },
		{ ParsedPageInfo::PageLinksColumn::AltOrTitleLengthColumn, QObject::tr("Alt Text Length") },
		{ ParsedPageInfo::PageLinksColumn::LinkParameterColumn, QObject::tr("Nofollow / Dofollow") }
	};

	return s_pageLinksColumns.value(column, QString::null);
}

int ParsedPageInfo::columnPrefferedSize(Column column)
{
	static QMap<Column, int> s_parsedPageColumnPrefferedSizes =
	{
		{ ParsedPageInfo::Column::UrlColumn, QuickieWebBotHelpers::pointsToPixels(300) },
		{ ParsedPageInfo::Column::TitleColumn, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::ContentTypeColumn, QuickieWebBotHelpers::pointsToPixels(160) },
		{ ParsedPageInfo::Column::MetaRefreshColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::MetaRobotsColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::MetaDescriptionColumn, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::MetaKeywordsColumn, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::RedirectedUrlColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::ServerResponseColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::FirstH1Column, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::SecondH1Column, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::FirstH2Column, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::SecondH2Column, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::Column::CanonicalLinkElementColumn, QuickieWebBotHelpers::pointsToPixels(150) },
		{ ParsedPageInfo::Column::StatusCodeColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::PageSizeKbColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::WordCountColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::PageHashColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::TitleLengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::UrlLengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::MetaDescriptionLengthColumn, QuickieWebBotHelpers::pointsToPixels(110) },
		{ ParsedPageInfo::Column::MetaKeywordsLengthColumn, QuickieWebBotHelpers::pointsToPixels(110) },
		{ ParsedPageInfo::Column::FirstH1LengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::SecondH1LengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::FirstH2LengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::SecondH2LengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::Column::ImageSizeKbColumn, QuickieWebBotHelpers::pointsToPixels(100) },
	};

	const int result = s_parsedPageColumnPrefferedSizes.value(column, -1);

	ASSERT(result >= 0);

	return result;
}


int ParsedPageInfo::columnPrefferedSize(PageLinksColumn column)
{
	static QMap<PageLinksColumn, int> s_pageLinksColumnPrefferedSizes =
	{
		{ ParsedPageInfo::PageLinksColumn::UrlColumn, QuickieWebBotHelpers::pointsToPixels(300) },
		{ ParsedPageInfo::PageLinksColumn::StatusCodeColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::PageLinksColumn::AltOrTitleColumn, QuickieWebBotHelpers::pointsToPixels(400) },
		{ ParsedPageInfo::PageLinksColumn::AltOrTitleLengthColumn, QuickieWebBotHelpers::pointsToPixels(100) },
		{ ParsedPageInfo::PageLinksColumn::LinkParameterColumn, QuickieWebBotHelpers::pointsToPixels(150) }
	};

	const int result = s_pageLinksColumnPrefferedSizes.value(column, -1);

	ASSERT(result >= 0);

	return result;
}

QVariant ParsedPageInfo::itemValue(Column column) const
{
	if (!m_parsedPage)
	{
		return QVariant();
	}

	return (this->*acceptItemMethod(column))();
}

QVariant ParsedPageInfo::itemValue(PageLinksColumn pageLinksColumn, PageLinkContext context, size_t number) const
{
	const PageLinksPointer pointer = pointerByContext(context);

	const WebCrawler::ResourceLink& resourceLink = (m_parsedPage.get()->*pointer)[number];

	if (isPageLinksColumnMappedToParsedPageColumn(pageLinksColumn))
	{
		//
		// It looks like a bad solution and need speed optimization
		//
		ParsedPageInfo parsedPageInfo(resourceLink.resource.lock());

		return parsedPageInfo.itemValue(mapPageLinksColumnToParsedPageColumn(pageLinksColumn));
	}

	QVariant value;

	switch (pageLinksColumn)
	{
		case PageLinksColumn::AltOrTitleColumn:
		{
			value = resourceLink.altOrTitle;
			break;
		}
		case PageLinksColumn::LinkParameterColumn:
		{
			value = linkParameterDescription(resourceLink.urlParameter);
			break;
		}
	}

	return value;
}

size_t ParsedPageInfo::itemCount(PageLinkContext context) const
{
	if (!m_parsedPage)
	{
		return 0;
	}

	const PageLinksPointer pointer = pointerByContext(context);

	return (m_parsedPage.get()->*pointer).size();
}

bool ParsedPageInfo::isPageLinksColumnMappedToParsedPageColumn(PageLinksColumn pageLinksColumn) noexcept
{
	switch (pageLinksColumn)
	{
		case PageLinksColumn::UrlColumn:
		case PageLinksColumn::StatusCodeColumn:
		{
			return true;
		}
	}

	return false;
}

ParsedPageInfo::Column ParsedPageInfo::mapPageLinksColumnToParsedPageColumn(PageLinksColumn pageLinksColumn) noexcept
{
	DEBUG_ASSERT(isPageLinksColumnMappedToParsedPageColumn(pageLinksColumn));

	switch (pageLinksColumn)
	{
		case PageLinksColumn::UrlColumn:
		{
			return Column::UrlColumn;
		}

		case PageLinksColumn::StatusCodeColumn:
		{
			return Column::StatusCodeColumn;
		}
		default:
		{
			DEBUG_ASSERT(!"What happens?");
		}
	}

	return Column();
}

ParsedPageInfo::PageLinksPointer ParsedPageInfo::pointerByContext(PageLinkContext context)
{
	PageLinksPointer pointer = nullptr;

	switch (context)
	{
		case PageLinkContext::LinksOnThisPage:
		{
			pointer = &WebCrawler::ParsedPage::linksOnThisPage;
			break;
		}
		case PageLinkContext::LinksToThisPage:
		{
			pointer = &WebCrawler::ParsedPage::linksToThisPage;
			break;
		}
		default:
		{
			ASSERT(pointer);
		}
	}

	return pointer;
}

QString ParsedPageInfo::linkParameterDescription(WebCrawler::LinkParameter linkParameter)
{
	switch (linkParameter)
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

ParsedPageInfo::MethodAcceptor ParsedPageInfo::acceptItemMethod(Column column)
{
	switch (column)
	{
		case Column::UrlColumn: 
		{
			return &ParsedPageInfo::acceptUrl;
		}
		case Column::ContentTypeColumn: 
		{
			return &ParsedPageInfo::acceptContentType;
		}
		case Column::TitleColumn: 
		{
			return &ParsedPageInfo::acceptTitle;
		}
		case Column::MetaRefreshColumn: 
		{
			return &ParsedPageInfo::acceptMetaRefresh;
		}
		case Column::MetaRobotsColumn: 
		{
			return &ParsedPageInfo::acceptMetaRobots;
		}
		case Column::RedirectedUrlColumn: 
		{
			return &ParsedPageInfo::acceptMetaRobots;
		}
		case Column::ServerResponseColumn: 
		{
			return &ParsedPageInfo::acceptServerResponse;
		}
		case Column::MetaDescriptionColumn: 
		{
			return &ParsedPageInfo::acceptMetaDescription;
		}
		case Column::MetaKeywordsColumn: 
		{
			return &ParsedPageInfo::acceptMetaKeywords;
		}
		case Column::FirstH1Column: 
		{
			return &ParsedPageInfo::acceptFirstH1;
		}
		case Column::SecondH1Column: 
		{
			return &ParsedPageInfo::acceptSecondH1;
		}
		case Column::FirstH2Column: 
		{
			return &ParsedPageInfo::acceptFirstH2;
		}
		case Column::SecondH2Column: 
		{
			return &ParsedPageInfo::acceptSecondH2;
		}
		case Column::CanonicalLinkElementColumn: 
		{
			return &ParsedPageInfo::acceptCanonicalLinkElement;
		}
		case Column::StatusCodeColumn: 
		{
			return &ParsedPageInfo::acceptStatusCode;
		}
		case Column::UrlLengthColumn: 
		{
			return &ParsedPageInfo::acceptUrlLength;
		}
		case Column::TitleLengthColumn: 
		{
			return &ParsedPageInfo::acceptTitleLength;
		}
		case Column::MetaDescriptionLengthColumn: 
		{
			return &ParsedPageInfo::acceptMetaDescriptionLength;
		}
		case Column::MetaKeywordsLengthColumn: 
		{
			return &ParsedPageInfo::acceptMetaKeywordsLength;
		}
		case Column::FirstH1LengthColumn: 
		{
			return &ParsedPageInfo::acceptFirstH1Length;
		}
		case Column::SecondH1LengthColumn: 
		{
			return &ParsedPageInfo::acceptSecondH1Length;
		}
		case Column::FirstH2LengthColumn: 
		{
			return &ParsedPageInfo::acceptFirstH2Length;
		}
		case Column::SecondH2LengthColumn: 
		{
			return &ParsedPageInfo::acceptSecondH2Length;
		}
		case Column::PageSizeKbColumn: 
		{
			return &ParsedPageInfo::acceptPageSizeKb;
		}
		case Column::WordCountColumn: 
		{
			return &ParsedPageInfo::acceptWordCount;
		}
		case Column::PageHashColumn: 
		{
			return &ParsedPageInfo::acceptPageHash;
		}
		case Column::ImageSizeKbColumn: 
		{
			return &ParsedPageInfo::acceptImageSizeKb;
		}
		default:
		{
			ASSERT(!"Unknown element");
		}
	}

	return MethodAcceptor();
}

QVariant ParsedPageInfo::acceptUrl() const
{
	return m_parsedPage->url;
}

QVariant ParsedPageInfo::acceptContentType() const
{
	return m_parsedPage->contentType;
}

QVariant ParsedPageInfo::acceptTitle() const
{
	return m_parsedPage->title;
}

QVariant ParsedPageInfo::acceptMetaRefresh() const
{
	return m_parsedPage->metaRefresh;
}

QVariant ParsedPageInfo::acceptMetaRobots() const
{
	static std::map<int, QString> s_cache;

	const WebCrawler::MetaRobotsFlags flags = m_parsedPage->metaRobotsFlags;

	auto it = s_cache.find(flags);
	if (it != s_cache.end())
	{
		return it->second;
	}
	
	QString result;
	auto addFlag = [&flags, &result](WebCrawler::MetaRobotsItem flag, const QString& flagStr)
	{
		if (flags.testFlag(flag))
		{
			result += result.isEmpty() ? flagStr : QString(", ") + flagStr;
		}
	};
	
	addFlag(WebCrawler::MetaRobotsAll, QString("all"));
	addFlag(WebCrawler::MetaRobotsNoIndex, QString("noindex"));
	addFlag(WebCrawler::MetaRobotsNoFollow, QString("nofollow"));
	addFlag(WebCrawler::MetaRobotsNone, QString("none"));
	addFlag(WebCrawler::MetaRobotsNoArchive, QString("noarchive"));
	addFlag(WebCrawler::MetaRobotsNoSnippet, QString("nosnippet"));
	addFlag(WebCrawler::MetaRobotsNoODP, QString("noodp"));
	addFlag(WebCrawler::MetaRobotsNoTranslate, QString("notranslate"));
	addFlag(WebCrawler::MetaRobotsNoImageIndex, QString("noimageindex"));
	addFlag(WebCrawler::MetaRobotsIndex, QString("index"));
	addFlag(WebCrawler::MetaRobotsFollow, QString("follow"));
	addFlag(WebCrawler::MetaRobotsNoYaCa, QString("noyaca"));
	addFlag(WebCrawler::MetaRobotsNoYDir, QString("noydir"));

	s_cache[flags] = result;
	return result;
}

QVariant ParsedPageInfo::acceptRedirectedUrl() const
{
	return m_parsedPage->redirectedUrl;
}

QVariant ParsedPageInfo::acceptServerResponse() const
{
	return m_parsedPage->serverResponse;
}

QVariant ParsedPageInfo::acceptMetaDescription() const
{
	return m_parsedPage->metaDescription;
}

QVariant ParsedPageInfo::acceptMetaKeywords() const
{
	return m_parsedPage->metaKeywords;
}

QVariant ParsedPageInfo::acceptFirstH1() const
{
	return m_parsedPage->firstH1;
}

QVariant ParsedPageInfo::acceptSecondH1() const
{
	return m_parsedPage->secondH1;
}

QVariant ParsedPageInfo::acceptFirstH2() const
{
	return m_parsedPage->firstH2;
}

QVariant ParsedPageInfo::acceptSecondH2() const
{
	return m_parsedPage->secondH2;
}

QVariant ParsedPageInfo::acceptCanonicalLinkElement() const
{
	return m_parsedPage->canonicalLinkElement;
}

QVariant ParsedPageInfo::acceptStatusCode() const
{
	return Common::StatusCodeDescription::description(m_parsedPage->statusCode);
}

QVariant ParsedPageInfo::acceptUrlLength() const
{
	return m_parsedPage->url.toString().length();
}

QVariant ParsedPageInfo::acceptTitleLength() const
{
	return m_parsedPage->title.size();
}

QVariant ParsedPageInfo::acceptMetaDescriptionLength() const
{
	return m_parsedPage->metaDescription.size();
}

QVariant ParsedPageInfo::acceptMetaKeywordsLength() const
{
	return m_parsedPage->metaKeywords.size();
}

QVariant ParsedPageInfo::acceptFirstH1Length() const
{
	return m_parsedPage->firstH1.size();
}

QVariant ParsedPageInfo::acceptSecondH1Length() const
{
	return m_parsedPage->secondH1.size();
}

QVariant ParsedPageInfo::acceptFirstH2Length() const
{
	return m_parsedPage->firstH2.size();
}

QVariant ParsedPageInfo::acceptSecondH2Length() const
{
	return m_parsedPage->secondH2.size();
}

QVariant ParsedPageInfo::acceptPageSizeKb() const
{
	return m_parsedPage->pageSizeKilobytes;
}

QVariant ParsedPageInfo::acceptWordCount() const
{
	return m_parsedPage->wordCount;
}

QVariant ParsedPageInfo::acceptPageHash() const
{
	return m_parsedPage->pageHash;
}

QVariant ParsedPageInfo::acceptImageSizeKb() const
{
	return m_parsedPage->pageSizeKilobytes;
}

void ParsedPageInfo::checkColumnType(Column column)
{
	ASSERT(column > Column::BeginEnumPageInfoColumn && column < Column::EndEnumPageInfoColumn);
}

}
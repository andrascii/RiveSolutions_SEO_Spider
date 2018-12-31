#include "page_data_collector.h"
#include "crawler_options.h"
#include "page_parser_helpers.h"
#include "html_resources_parser.h"
#include "js_resources_parser.h"
#include "css_resources_parser.h"
#include "images_resources_parser.h"
#include "video_resources_parser.h"
#include "flash_resources_parser.h"
#include "other_resources_parser.h"
#include "ym_parser.h"
#include "base_url_parser.h"
#include "status_code.h"
#include "download_response.h"
#include "gumbo_html_parser.h"
#include "myhtml_parser.h"

namespace CrawlerEngine
{

PageDataCollector::PageDataCollector(QObject* parent)
	: QObject(parent)
	, m_htmlParser(std::make_shared<MyHtmlParser>())
{
}

void PageDataCollector::setOptions(const CrawlerOptionsData& crawlerOptionsData) noexcept
{
	m_crawlerOptionsData = crawlerOptionsData;

	applyOptions();
}

std::vector<ParsedPagePtr> PageDataCollector::collectPageDataFromResponse(const HopsChain& hopsChain)
{
	std::vector<ParsedPagePtr> pages(hopsChain.length());
	std::generate(pages.begin(), pages.end(), [] { return std::make_shared<ParsedPage>(); });

	for (std::size_t i = 0; i < pages.size(); ++i)
	{
		DEBUG_ASSERT(hopsChain[i].url().isValid());
		if (hopsChain[i].url().isValid())
		{
			collectReplyData(hopsChain[i], pages[i]);
		}
	}

	const auto collectEachPageData = [&](const Hop& hop, ParsedPagePtr& page)
	{
		m_htmlParser->parseHtmlPage(hop.body(), hop.responseHeaders());

		if (page->resourceType == ResourceType::ResourceImage)
		{
			page->rawResponse = hop.body();
		}

		m_parser.parse(hop.responseHeaders(), page);
	};

	for (std::size_t i = 0; i < pages.size(); ++i)
	{
		collectEachPageData(hopsChain[i], pages[i]);
	}

	return pages;
}

void PageDataCollector::applyOptions()
{
	m_parser.clear();

	m_parser.addParser(std::make_shared<BaseUrlParser>(m_htmlParser.get()));
	m_parser.addParser(std::make_shared<HtmlResourcesParser>(m_htmlParser.get(), m_crawlerOptionsData.crawlMetaHrefLangLinks));

	if (m_crawlerOptionsData.parserTypeFlags.testFlag(JavaScriptResourcesParserType))
	{
		m_parser.addParser(createParser(JavaScriptResourcesParserType));
	}
	if (m_crawlerOptionsData.parserTypeFlags.testFlag(CssResourcesParserType))
	{
		m_parser.addParser(createParser(CssResourcesParserType));
	}
	if (m_crawlerOptionsData.parserTypeFlags.testFlag(ImagesResourcesParserType))
	{
		m_parser.addParser(createParser(ImagesResourcesParserType));
	}
	if (m_crawlerOptionsData.parserTypeFlags.testFlag(VideoResourcesParserType))
	{
		m_parser.addParser(createParser(VideoResourcesParserType));
	}
	if (m_crawlerOptionsData.parserTypeFlags.testFlag(FlashResourcesParserType))
	{
		m_parser.addParser(createParser(FlashResourcesParserType));
	}
	if (m_crawlerOptionsData.parserTypeFlags.testFlag(OtherResourcesParserType))
	{
		m_parser.addParser(createParser(OtherResourcesParserType));
	}

	if (!m_crawlerOptionsData.searchYandexMetricaCounters)
	{
		return;
	}

	if (m_crawlerOptionsData.searchYandexMetricaCounter1)
	{
		m_parser.addParser(std::make_shared<YmParser>(
			m_htmlParser.get(),
			m_crawlerOptionsData.yandexMetricaCounter1Id,
			StorageType::YandexMetricaCounter1StorageType));
	}
	if (m_crawlerOptionsData.searchYandexMetricaCounter2)
	{
		m_parser.addParser(std::make_shared<YmParser>(
			m_htmlParser.get(),
			m_crawlerOptionsData.yandexMetricaCounter2Id,
			StorageType::YandexMetricaCounter2StorageType));
	}
	if (m_crawlerOptionsData.searchYandexMetricaCounter3)
	{
		m_parser.addParser(std::make_shared<YmParser>(
			m_htmlParser.get(),
			m_crawlerOptionsData.yandexMetricaCounter3Id,
			StorageType::YandexMetricaCounter3StorageType));
	}
	if (m_crawlerOptionsData.searchYandexMetricaCounter4)
	{
		m_parser.addParser(std::make_shared<YmParser>(
			m_htmlParser.get(),
			m_crawlerOptionsData.yandexMetricaCounter4Id,
			StorageType::YandexMetricaCounter4StorageType));
	}
	if (m_crawlerOptionsData.searchYandexMetricaCounter5)
	{
		m_parser.addParser(std::make_shared<YmParser>(
			m_htmlParser.get(),
			m_crawlerOptionsData.yandexMetricaCounter5Id,
			StorageType::YandexMetricaCounter5StorageType));
	}
}

Url PageDataCollector::resolveRedirectUrl(const Hop& hop)
{
	Url redirectUrl;

	if (hop.redirectUrl().isEmpty() || !hop.redirectUrl().isRelative())
	{
		redirectUrl = hop.redirectUrl();
	}
	else
	{
		redirectUrl = PageParserHelpers::resolveRelativeUrl(hop.redirectUrl(), hop.url());
	}

	return redirectUrl;
}

void PageDataCollector::collectReplyData(const Hop& hop, ParsedPagePtr& page) const
{
	page->url = hop.url();

	DEBUG_ASSERT(page->url.isValid());

	page->statusCode = static_cast<Common::StatusCode>(hop.statusCode());

	page->pageSizeKilobytes = hop.body().size() / 1024;

	page->serverResponse = hop.responseHeaders().makeString();

	page->pageHash = std::hash<std::string>()(hop.body().toStdString());

	page->isThisExternalPage = PageParserHelpers::isUrlExternal(
		m_crawlerOptionsData.startCrawlingPage, page->url, m_crawlerOptionsData.checkSubdomains);

	page->responseTime = hop.elapsedTime();

	const std::vector<QString> contentTypeValues = hop.responseHeaders().valueOf("content-type");

	page->contentType = contentTypeValues.empty() ? QString() : contentTypeValues.front();

	if (contentTypeValues.size() > 1)
	{
		WARNLOG << page->url.toDisplayString() << "contains several content-type headers from HTTP response";
	}

	setResourceType(page);

	page->redirectedUrl = resolveRedirectUrl(hop);

	const std::vector<QString> dateHeaders = hop.responseHeaders().valueOf(QString("Date"));

	if (!dateHeaders.empty())
	{
		const QString dateHeader = dateHeaders[0];
		page->responseDate = QDateTime::fromString(dateHeader, Qt::RFC2822Date);

		if (!page->responseDate.isValid())
		{
			INFOLOG << "Unable to convert date" << dateHeader;
		}
	}

	const std::vector<QString> lastModifiedHeaders = hop.responseHeaders().valueOf(QString("Last-Modified"));

	if (!lastModifiedHeaders.empty())
	{
		const QString lastModifiedHeader = lastModifiedHeaders[0];
		page->lastModifiedDate = QDateTime::fromString(lastModifiedHeader, Qt::RFC2822Date);

		if (!page->lastModifiedDate.isValid())
		{
			INFOLOG << "Unable to convert date" << lastModifiedHeader;
		}
	}
}

void PageDataCollector::setResourceType(ParsedPagePtr& page) const
{
	if (page->contentType.contains("javascript"))
	{
		page->resourceType = ResourceType::ResourceJavaScript;
		return;
	}

	if (page->contentType.startsWith("image/"))
	{
		page->resourceType = ResourceType::ResourceImage;
		return;
	}

	if (page->contentType == "text/css")
	{
		page->resourceType = ResourceType::ResourceStyleSheet;
		return;
	}

	if (PageParserHelpers::isHtmlOrPlainContentType(page->contentType))
	{
		page->resourceType = ResourceType::ResourceHtml;
		return;
	}

	WARNLOG << "Unknown resource type: " << page->contentType;

	page->resourceType = ResourceType::ResourceOther;
}

std::shared_ptr<IPageParser> PageDataCollector::createParser(ParserType parserType) const
{
	switch (parserType)
	{
		case JavaScriptResourcesParserType:
		{
			return std::make_shared<JsResourcesParser>(m_htmlParser.get());
		}
		case CssResourcesParserType:
		{
			return std::make_shared<CssResourcesParser>(m_htmlParser.get());
		}
		case ImagesResourcesParserType:
		{
			return std::make_shared<ImagesResourcesParser>(m_htmlParser.get());
		}
		case VideoResourcesParserType:
		{
			return std::make_shared<VideoResourcesParser>(m_htmlParser.get());
		}
		case FlashResourcesParserType:
		{
			return std::make_shared<FlashResourcesParser>(m_htmlParser.get());
		}
		case OtherResourcesParserType:
		{
			return std::make_shared<OtherResourcesParser>(m_htmlParser.get());
		}
		default:
		{
			ASSERT(!"Undefined parser type");
		}
	}

	return std::shared_ptr<IPageParser>(nullptr);
}

}

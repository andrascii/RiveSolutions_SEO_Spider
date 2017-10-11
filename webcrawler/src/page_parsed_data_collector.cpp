#include "page_parsed_data_collector.h"
#include "crawler_options.h"
#include "page_parser_helpers.h"
#include "gumbo_parsing_helpers.h"
#include "html_resources_parser.h"
#include "js_resources_parser.h"
#include "css_resources_parser.h"
#include "images_resources_parser.h"
#include "video_resources_parser.h"
#include "flash_resources_parser.h"
#include "status_code.h"

namespace WebCrawler
{

PageParsedDataCollector::PageParsedDataCollector(QObject* parent)
	: QObject(parent)
{
}

void PageParsedDataCollector::setOptions(const CrawlerOptions& options) noexcept
{
	assert(m_options.minTitleLength <= m_options.maxTitleLength);

	m_options = options;

	applyOptions();
}

const CrawlerOptions& PageParsedDataCollector::options() const noexcept
{
	return m_options;
}

ParsedPagePtr PageParsedDataCollector::collectPageDataFromReply(const QueuedDownloader::Reply& reply)
{
	ParsedPagePtr page(new ParsedPage);

	collectReplyData(reply, page);

	if (page->pageSizeKilobytes > 512)
	{
		WARNINGLOG << "Page size is greater than 512 KB,"
			<< "size:" << page->pageSizeKilobytes << "KB,"
			<< "URL:" << reply.url.toString() << ","
			<< "Content-Type:" << page->contentType;
	}

	QByteArray decodedHtmlPage;
	if (page->resourceType == ResourceType::ResourceHtml)
	{
		decodedHtmlPage = GumboParsingHelpers::decodeHtmlPage(reply.responseBody);
#ifdef QT_DEBUG
		page->rawResponse = qCompress(decodedHtmlPage, 9);
#endif
	}
	else if (page->resourceType == ResourceType::ResourceImage)
	{
		page->rawResponse = reply.responseBody;
	}

	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	collectParsedPageData(gumboOutput.output(), page);
	collectUrlList(gumboOutput.output());

	return page;
}

const std::vector<QUrl>& PageParsedDataCollector::urlList() const noexcept
{
	return m_urlList;
}

void PageParsedDataCollector::applyOptions()
{
	m_parser.clear();

	if (m_options.parserTypeFlags.testFlag(HtmlResourcesParserType))
	{
		m_parser.addParser(createParser(HtmlResourcesParserType));
	}
	if (m_options.parserTypeFlags.testFlag(JavaScriptResourcesParserType))
	{
		m_parser.addParser(createParser(JavaScriptResourcesParserType));
	}
	if (m_options.parserTypeFlags.testFlag(CssResourcesParserType))
	{
		m_parser.addParser(createParser(CssResourcesParserType));
	}
	if (m_options.parserTypeFlags.testFlag(ImagesResourcesParserType))
	{
		m_parser.addParser(createParser(ImagesResourcesParserType));
	}
	if (m_options.parserTypeFlags.testFlag(VideoResourcesParserType))
	{
		m_parser.addParser(createParser(VideoResourcesParserType));
	}
	if (m_options.parserTypeFlags.testFlag(FlashResourcesParserType))
	{
		m_parser.addParser(createParser(FlashResourcesParserType));
	}
}

bool PageParsedDataCollector::isParserEnabled(int flags) const noexcept
{
	return true;
}

QUrl PageParsedDataCollector::resolveRedirectUrl(const QueuedDownloader::Reply& reply) const
{
	if (reply.redirectUrl.isEmpty() || !reply.redirectUrl.isRelative())
	{
		return reply.redirectUrl;
	}

	return PageParserHelpers::resolveRelativeUrl(reply.redirectUrl, reply.url);
}

void PageParsedDataCollector::collectReplyData(const QueuedDownloader::Reply& reply, ParsedPagePtr& page) const
{
	page->url = reply.url;

	page->statusCode = static_cast<Common::StatusCode>(reply.statusCode);

	page->pageSizeKilobytes = reply.responseBody.size() / 1024;

	page->serverResponse = reply.responseHeaderValuePairs;

	page->pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());

	page->isThisExternalPage = PageParserHelpers::isUrlExternal(options().host, page->url);

	page->contentType = QString();

	foreach(QByteArray key, reply.responseHeaders.keys())
	{
		if (key.toLower() != "content-type")
		{
			continue;
		}

		page->contentType = reply.responseHeaders.value(key);
		break;
	}

	setResourceCategory(page);

	page->redirectedUrl = resolveRedirectUrl(reply);
}

void PageParsedDataCollector::collectParsedPageData(GumboOutput* output, ParsedPagePtr& page)
{
	m_parser.parse(output, page);
}

void PageParsedDataCollector::collectUrlList(GumboOutput* output)
{
	m_urlList.clear();

	std::vector<LinkInfo> linksOnPage = GumboParsingHelpers::parsePageUrlList(output->root, true);

	for (const LinkInfo& linkInfo : linksOnPage)
	{
		m_urlList.push_back(linkInfo.url);
	}
}

void PageParsedDataCollector::setResourceCategory(ParsedPagePtr& page) const
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

	if (PageParserHelpers::isHtmlContentType(page->contentType))
	{
		page->resourceType = ResourceType::ResourceHtml;
		return;
	}

	WARNINGLOG << "Unknown resource type: " << page->contentType;

	page->resourceType = ResourceType::ResourceOther;
}

std::shared_ptr<WebCrawler::IPageParser> 
PageParsedDataCollector::createParser(ParserType parserType) const
{
	switch (parserType)
	{
		case HtmlResourcesParserType:
		{
			return std::make_shared<HtmlResourcesParser>(&m_resourcesCache);
		}
		case JavaScriptResourcesParserType:
		{
			return std::make_shared<JsResourcesParser>(&m_resourcesCache);
		}
		case CssResourcesParserType:
		{
			return std::make_shared<CssResourcesParser>(&m_resourcesCache);
		}
		case ImagesResourcesParserType:
		{
			return std::make_shared<ImagesResourcesParser>(&m_resourcesCache);
		}
		case VideoResourcesParserType:
		{
			return std::make_shared<VideoResourcesParser>(&m_resourcesCache);
		}
		case FlashResourcesParserType:
		{
			return std::make_shared<FlashResourcesParser>(&m_resourcesCache);
		}
	}

	ASSERT(!"Undefined parser type");
	return std::shared_ptr<IPageParser>(nullptr);
}

}

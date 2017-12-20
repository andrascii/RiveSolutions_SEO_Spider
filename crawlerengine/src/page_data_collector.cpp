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
#include "status_code.h"
#include "gumbo_parsing_helpers.h"
#include "download_response.h"

namespace CrawlerEngine
{

PageDataCollector::PageDataCollector(QObject* parent)
	: QObject(parent)
{
}

void PageDataCollector::setOptions(const CrawlerOptions& crawlerOptions) noexcept
{
	m_crawlerOptions = crawlerOptions;

	applyOptions();
}

ParsedPagePtr PageDataCollector::collectPageDataFromResponse(const DownloadResponse& response)
{
	ParsedPagePtr page(new ParsedPage);

	collectReplyData(response, page);

	if (page->pageSizeKilobytes > 512)
	{
		WARNLOG << "Page size is greater than 512 KB,"
			<< "size:" << page->pageSizeKilobytes << "KB,"
			<< "URL:" << response.url.toString() << ","
			<< "Content-Type:" << page->contentType;
	}

	QByteArray decodedHtmlPage;
	if (page->resourceType == ResourceType::ResourceHtml)
	{
		decodedHtmlPage = GumboParsingHelpers::decodeHtmlPage(response.responseBody, response.responseHeaders);
#ifdef QT_DEBUG
		page->rawResponse = qCompress(decodedHtmlPage, 9);
#endif
	}
	else if (page->resourceType == ResourceType::ResourceImage)
	{
		page->rawResponse = response.responseBody;
	}

	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	collectParsedPageData(gumboOutput.output(), response.responseHeaders, page);
	collectUrlList(gumboOutput.output());

	return page;
}

const std::vector<LinkInfo>& PageDataCollector::outlinks() const noexcept
{
	return m_outlinks;
}

void PageDataCollector::applyOptions()
{
	m_parser.clear();

	m_parser.addParser(std::make_shared<HtmlResourcesParser>());

	if (m_crawlerOptions.parserTypeFlags.testFlag(JavaScriptResourcesParserType))
	{
		m_parser.addParser(createParser(JavaScriptResourcesParserType));
	}
	if (m_crawlerOptions.parserTypeFlags.testFlag(CssResourcesParserType))
	{
		m_parser.addParser(createParser(CssResourcesParserType));
	}
	if (m_crawlerOptions.parserTypeFlags.testFlag(ImagesResourcesParserType))
	{
		m_parser.addParser(createParser(ImagesResourcesParserType));
	}
	if (m_crawlerOptions.parserTypeFlags.testFlag(VideoResourcesParserType))
	{
		m_parser.addParser(createParser(VideoResourcesParserType));
	}
	if (m_crawlerOptions.parserTypeFlags.testFlag(FlashResourcesParserType))
	{
		m_parser.addParser(createParser(FlashResourcesParserType));
	}
	if (m_crawlerOptions.parserTypeFlags.testFlag(OtherResourcesParserType))
	{
		m_parser.addParser(createParser(OtherResourcesParserType));
	}
}

CustomUrl PageDataCollector::resolveRedirectUrl(const DownloadResponse& response)
{
	CustomUrl redirectUrl;

	if (response.redirectUrl.isEmpty() || !response.redirectUrl.isRelative())
	{
		redirectUrl = response.redirectUrl;
	}
	else
	{
		PageParserHelpers::resolveRelativeUrl(response.redirectUrl, response.url);
	}

	return redirectUrl;
}

void PageDataCollector::collectReplyData(const DownloadResponse& response, ParsedPagePtr& page) const
{
	page->url = response.url;

	page->statusCode = static_cast<Common::StatusCode>(response.statusCode);

	page->pageSizeKilobytes = response.responseBody.size() / 1024;

	page->serverResponse = response.responseHeaders.makeString();

	page->pageHash = std::hash<std::string>()(response.responseBody.toStdString().c_str());

	page->isThisExternalPage = PageParserHelpers::isUrlExternal(m_crawlerOptions.host, page->url);

	const std::vector<QString> contentTypeValues = response.responseHeaders.valueOf("content-type");
	page->contentType = contentTypeValues.empty() ? QString() : contentTypeValues.front();

	if (contentTypeValues.size() > 1)
	{
		WARNLOG << page->url.toDisplayString() << "contains several content-type headers from HTTP response";
	}

	setResourceCategory(page);

	page->redirectedUrl = resolveRedirectUrl(response);

	const std::vector<QString> dateHeaders = response.responseHeaders.valueOf(QString("Date"));
	if (!dateHeaders.empty())
	{
		const QString dateHeader = dateHeaders[0];
		page->responseDate = QDateTime::fromString(dateHeader, Qt::RFC2822Date);
		if (!page->responseDate.isValid())
		{
			INFOLOG << "Unable to convert date" << dateHeader;
		}
	}

	const std::vector<QString> lastModifiedHeaders = response.responseHeaders.valueOf(QString("Last-Modified"));
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

void PageDataCollector::collectParsedPageData(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	m_parser.parse(output, headers, page);
}

void PageDataCollector::collectUrlList(GumboOutput* output)
{
	m_outlinks.clear();
	m_outlinks = GumboParsingHelpers::parsePageUrlList(output->root, true);
}

void PageDataCollector::setResourceCategory(ParsedPagePtr& page) const
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
			return std::make_shared<JsResourcesParser>();
		}
		case CssResourcesParserType:
		{
			return std::make_shared<CssResourcesParser>();
		}
		case ImagesResourcesParserType:
		{
			return std::make_shared<ImagesResourcesParser>();
		}
		case VideoResourcesParserType:
		{
			return std::make_shared<VideoResourcesParser>();
		}
		case FlashResourcesParserType:
		{
			return std::make_shared<FlashResourcesParser>();
		}
		case OtherResourcesParserType:
		{
			return std::make_shared<OtherResourcesParser>();
		}
		default:
		{
			ASSERT(!"Undefined parser type");
		}
	}

	return std::shared_ptr<IPageParser>(nullptr);
}

}

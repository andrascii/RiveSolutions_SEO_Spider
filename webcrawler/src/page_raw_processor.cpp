#include "page_raw_processor.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"
#include "html_page_h_parser.h"
#include "html_page_word_count_parser.h"
#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_raw_parser_helpers.h"

namespace WebCrawler
{

PageRawProcessor::PageRawProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, 
	QueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this, QByteArray("PageRawProcessorThread"))
	, m_webCrawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
{
	moveThisToSeparateThread();

	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageResourcesParser>()); // should be first in the list
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageMetaParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageTitleParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageHParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageWordCountParser>());
	
}

void PageRawProcessor::setHost(QUrl host)
{
	m_host = host;
}

void PageRawProcessor::process()
{
	WebCrawlerRequest url;
	QueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_webCrawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		PageRawPtr pageRaw(new PageRaw);

		int sizeKB = reply.responseBody.size() / 1024;
		pageRaw->pageSizeKb = sizeKB;
		
		size_t pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());
		const QByteArray contentType = reply.responseHeaders.value("Content-Type", "");

		pageRaw->url = reply.url;
		pageRaw->statusCode = reply.statusCode;
		pageRaw->serverResponse = reply.responseHeaderValuePairs;
		pageRaw->pageHash = pageHash;
		pageRaw->contentType = contentType;
		pageRaw->hasSeveralH1Tags = false;
		pageRaw->hasSeveralH2Tags = false;
		pageRaw->hasSeveralMetaDescriptionTags = false;
		pageRaw->hasSeveralMetaKeywordsTags = false;
		pageRaw->hasSeveralTitleTags = false;


		if (sizeKB > 512)
		{
			WARNINGLOG << "Page size is greater than 512 KB, "
				<< "size: " << sizeKB << " KB, "
				<< "URL: " << reply.url.toString() << ", "
				<< "Content-Type: " << contentType;
		}
		
		m_htmlPageParser.parsePage(reply.responseBody, pageRaw);

		if (!PageRawParserHelpers::isUrlExternal(m_host, pageRaw->url))
		{
			std::vector<QUrl> urlList = m_htmlPageParser.pageUrlList();
			urlList = PageRawParserHelpers::resolveUrlList(reply.url, urlList);
			m_webCrawlerInternalUrlStorage->saveUrlList(urlList, RequestTypeGet);
		}

#ifdef QT_DEBUG
		if (pageRaw->fromUrl.toString().isEmpty())
		{
			pageRaw->rawHtml = qCompress(GumboParsingHelpers::decodeHtmlPage(reply.responseBody), 9);
		}
#endif // DEBUG

		Q_EMIT webPageParsed(pageRaw);
	}
}



}
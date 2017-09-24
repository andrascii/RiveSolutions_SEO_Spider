#include "crawler_worker_thread.h"
#include "crawler_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"
#include "html_page_h_parser.h"
#include "html_page_word_count_parser.h"
#include "html_page_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "iqueued_dowloader.h"

namespace WebCrawler
{

CrawlerWorkerThread::CrawlerWorkerThread(CrawlerUrlStorage* crawlerStorage, 
	IQueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this, QByteArray("CrawlerWorkerThread"))
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

void CrawlerWorkerThread::setHost(QUrl host)
{
	m_host = host;
}

void CrawlerWorkerThread::process()
{
	CrawlerRequest url;
	IQueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_webCrawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		ParsedPagePtr page(new ParsedPage);

		int sizeKB = reply.responseBody.size() / 1024;
		page->pageSizeKb = sizeKB;
		
		size_t pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());
		const QByteArray contentType = reply.responseHeaders.value("Content-Type", "");

		page->url = reply.url;
		page->statusCode = reply.statusCode;
		page->serverResponse = reply.responseHeaderValuePairs;
		page->pageHash = pageHash;
		page->contentType = contentType;
		page->hasSeveralH1Tags = false;
		page->hasSeveralH2Tags = false;
		page->hasSeveralMetaDescriptionTags = false;
		page->hasSeveralMetaKeywordsTags = false;
		page->hasSeveralTitleTags = false;
		page->isThisExternalPage = PageParserHelpers::isUrlExternal(m_host, page->url);


		if (sizeKB > 512)
		{
			WARNINGLOG << "Page size is greater than 512 KB, "
				<< "size: " << sizeKB << " KB, "
				<< "URL: " << reply.url.toString() << ", "
				<< "Content-Type: " << contentType;
		}

		preprocessRedirect(page, reply.redirectUrl);
		m_htmlPageParser.parsePage(reply.responseBody, page);
		schedulePageResourcesLoading(page);

#ifdef QT_DEBUG
		if (page->resourceType == ResourceType::ResourceHtml)
		{
			page->rawHtml = qCompress(GumboParsingHelpers::decodeHtmlPage(reply.responseBody), 9);
		}
#endif

		Q_EMIT pageParsed(page);
	}
}

void CrawlerWorkerThread::preprocessRedirect(const ParsedPagePtr& pageRaw, const QUrl& redirectUrl)
{
	if (redirectUrl.isEmpty())
	{
		return;
	}

	pageRaw->redirectedUrl = PageParserHelpers::resolveUrlList(pageRaw->url, std::vector<QUrl>{ redirectUrl }).front();
}

void CrawlerWorkerThread::schedulePageResourcesLoading(const ParsedPagePtr& pageRaw)
{
	if (pageRaw->isThisExternalPage)
	{
		return;
	}

	std::vector<QUrl> urlList = m_htmlPageParser.pageUrlList();
	urlList = PageParserHelpers::resolveUrlList(pageRaw->url, urlList);
	m_webCrawlerInternalUrlStorage->saveUrlList(urlList, RequestTypeGet);

	if (!pageRaw->redirectedUrl.isEmpty())
	{
		m_webCrawlerInternalUrlStorage->saveUrlList(std::vector<QUrl>{ pageRaw->redirectedUrl }, RequestTypeGet);
	}

	std::vector<QUrl> resourcesUrlList;

	for (const RawResourceOnPage& resource : pageRaw->allResourcesOnPage)
	{
		const QString resourceUrlStr = resource.thisResourceUrl.url.toDisplayString();

		if (PageParserHelpers::isHttpOrHttpsScheme(resourceUrlStr) &&
			resource.resourceType != ResourceType::ResourceHtml)
		{
			resourcesUrlList.push_back(resource.thisResourceUrl.url);
		}
	}

	m_webCrawlerInternalUrlStorage->saveUrlList(resourcesUrlList, RequestTypeHead);
}

}
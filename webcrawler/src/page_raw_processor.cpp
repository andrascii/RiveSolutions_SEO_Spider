#include "page_raw_processor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"
#include "html_page_h_parser.h"
#include "html_page_word_count_parser.h"


namespace WebCrawler
{

PageRawProcessor::PageRawProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this, QByteArray("PageRawProcessorThread"))
	, m_webCrawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
	, m_pageRaw(new PageRaw)
{
	moveThisToSeparateThread();

	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageMetaParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageTitleParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageHParser>());
	m_htmlPageParser.addPageRawParser(std::make_shared<HtmlPageWordCountParser>());
}

void PageRawProcessor::process()
{
	QUrl url;
	QueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_webCrawlerInternalUrlStorage->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		m_pageRaw.reset(new PageRaw);

		int sizeKB = reply.responseBody.size() / 1024;
		m_pageRaw->pageSizeKb = sizeKB;
		
		size_t pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());
		const QByteArray contentType = reply.responseHeaders.value("Content-Type", "");

		m_pageRaw->url = reply.url;
		m_pageRaw->statusCode = reply.statusCode;
		m_pageRaw->serverResponse = reply.responseHeaderValuePairs;
		m_pageRaw->pageHash = pageHash;

		if (!contentType.startsWith("application"))
		{
			// check for contentType != text/html ???
			// or check for existence html tags like <body> ???
			// or combine several approaches ???
			if (sizeKB > 512)
			{
				WARNINGLOG << "Page size is greater than 512 KB, "
					<< "size: " << sizeKB << " KB, "
					<< "URL: " << reply.url.toString() << ", "
					<< "Content-Type: " << contentType;
			}


			m_htmlPageParser.parsePage(reply.responseBody, m_pageRaw);
		}

		std::vector<QUrl> urlList = m_htmlPageParser.pageUrlList();

		resolveUrlList(reply.url, urlList);

		m_webCrawlerInternalUrlStorage->saveUrlList(urlList);

		Q_EMIT webPageParsed(m_pageRaw);
	}
}

void PageRawProcessor::resolveRelativeUrl(QUrl& relativeUrl, const QUrl& baseUrl)
{
	//
	// see: https://tools.ietf.org/html/rfc1808#section-4
	//

	assert(!baseUrl.isRelative() && "Base URL always MUST BE an absolute URL!!!");


	if (!relativeUrl.isRelative())
	{
		DEBUGLOG << "Passed non-relative url:" << relativeUrl.toDisplayString();

		return;
	}

	relativeUrl.setScheme(baseUrl.scheme());
	relativeUrl.setHost(baseUrl.host());

	QString pathWithoutFile = baseUrl.path();
	int lastSlashIndex = pathWithoutFile.lastIndexOf("/");

	if (relativeUrl.path().startsWith("/"))
	{
		//
		// Just exit from function because we already make it absolute
		//
		return;
	}
	else
	{
		//
		// Make path starts with slash for ensure valid QUrl behavior
		//
		relativeUrl.setPath("/" + relativeUrl.path());
	}

	if (relativeUrl.path().isEmpty())
	{
		relativeUrl.setPath(pathWithoutFile.left(lastSlashIndex + 1));
	}
	else
	{
		relativeUrl.setPath(pathWithoutFile.left(lastSlashIndex) + relativeUrl.path());
	}
}

void PageRawProcessor::resolveUrlList(const QUrl& baseUrl, std::vector<QUrl>& urlList) noexcept
{
	for (QUrl& url : urlList)
	{
		if (!url.isRelative())
		{
			continue;
		}

		resolveRelativeUrl(url, baseUrl);
	}
}

}
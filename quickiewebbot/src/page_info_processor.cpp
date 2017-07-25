#include "application.h"
#include "application_properties.h"
#include "page_info_processor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"
#include "html_page_h_parser.h"
#include "html_page_word_count_parser.h"


namespace QuickieWebBot
{

PageInfoProcessor::PageInfoProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, QueuedDownloader* queuedDownloader, QObject* parent)
	: AbstractThreadableObject(this, QByteArray("QuickieWebBot::PageInfoProcessorThread"))
	, m_webCrawlerInternalUrlStorage(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
	, m_pageInfo(new PageInfo)
{
	moveThisToSeparateThread();

	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageMetaParser>());
	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageTitleParser>());
	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageHParser>());
	m_htmlPageParser.addPageInfoParser(std::make_shared<HtmlPageWordCountParser>());
}

void PageInfoProcessor::process()
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
		m_pageInfo.reset(new PageInfo);

		m_htmlPageParser.parsePage(reply.responseBody, m_pageInfo);

		size_t pageHash = std::hash<std::string>()(reply.responseBody.toStdString().c_str());

		m_pageInfo->statusCode = reply.statusCode;
		m_pageInfo->url = reply.url;
		m_pageInfo->serverResponse = reply.responseHeaderValuePairs;
		m_pageInfo->pageHash = pageHash;

		std::vector<QUrl> urlList = m_htmlPageParser.pageUrlList();

		resolveUrlList(reply.url, urlList);

		m_webCrawlerInternalUrlStorage->saveUrlList(urlList);

		emit webPageParsed(m_pageInfo);
	}
}

void PageInfoProcessor::resolveRelativeUrl(QUrl& relativeUrl, const QUrl& baseUrl)
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

void PageInfoProcessor::resolveUrlList(const QUrl& baseUrl, std::vector<QUrl>& urlList) noexcept
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
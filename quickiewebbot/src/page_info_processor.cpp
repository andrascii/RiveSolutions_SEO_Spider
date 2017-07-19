#include "page_info_processor.h"
#include "service_locator.h"
#include "web_crawler_internal_url_storage.h"
#include "constants.h"
#include "html_page_title_parser.h"
#include "html_page_meta_parser.h"

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

		m_pageInfo->url = reply.url;
		m_pageInfo->serverResponse = reply.responseHeaderValuePairs;

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

	assert(!baseUrl.isRelative() && !"Base address always MUST BE an absolute URL!!!");


	if (!relativeUrl.isRelative())
	{
		DEBUGLOG() << "Passed non-relative url:" << relativeUrl.toDisplayString();

		return;
	}

	QRegularExpression pathWithoutFileRegexp;
	QRegularExpression fileRegexp;

	relativeUrl.setHost(baseUrl.host());

	if (relativeUrl.path().isEmpty())
	{
		//
		// remove file from baseUrl
		//
		relativeUrl.setPath(baseUrl.path());
		return;
	}

	if (relativeUrl.path().startsWith("/"))
	{
		return;
	}

	//
	// concat url paths
	//


	return;
}

void PageInfoProcessor::resolveUrlList(const QUrl& baseUrl, std::vector<QUrl>& urlList) noexcept
{
	for (QUrl& url : urlList)
	{
		if (url.isRelative())
		{
			resolveRelativeUrl(url, baseUrl);
			continue;
		}

		//
		// TODO: process absolute urls
		//
	}
}

}
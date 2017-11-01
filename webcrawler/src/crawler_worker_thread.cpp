#include "crawler_worker_thread.h"
#include "unique_link_store.h"
#include "page_parser_helpers.h"
#include "iqueued_dowloader.h"
#include "page_data_collector.h"
#include "options_link_filter.h"

namespace WebCrawler
{

CrawlerWorkerThread::CrawlerWorkerThread(UniqueLinkStore* crawlerStorage, IQueuedDownloader* queuedDownloader)
	: AbstractThreadableObject(this, QByteArray("CrawlerWorkerThread"))
	, m_pageDataCollector(new PageDataCollector(this))
	, m_uniqueLinkStore(crawlerStorage)
	, m_queuedDownloader(queuedDownloader)
{
	moveThisToSeparateThread();
}

void CrawlerWorkerThread::applyOptions(const CrawlerOptions& options, RobotsTxtRules robotsTxtRules)
{
	m_optionsLinkFilter.reset(new OptionsLinkFilter(options, robotsTxtRules));
	
	m_pageDataCollector->setOptions(options);
}

void CrawlerWorkerThread::process()
{
	CrawlerRequest url;
	IQueuedDownloader::Reply reply;

	const bool replyExtracted = m_queuedDownloader->extractReply(reply);
	const bool urlExtracted = m_uniqueLinkStore->extractUrl(url);

	if (urlExtracted)
	{
		m_queuedDownloader->scheduleUrl(url);
	}

	if (replyExtracted)
	{
		const ParsedPagePtr page = m_pageDataCollector->collectPageDataFromReply(reply);

		schedulePageResourcesLoading(page);

		emit pageParsed(page);
	}
}

void CrawlerWorkerThread::schedulePageResourcesLoading(const ParsedPagePtr& parsedPage) const
{
	if (parsedPage->isThisExternalPage)
	{
		return;
	}

	const auto isNofollowLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get()](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo) == OptionsLinkFilter::PermissionNofollowNotAllowed;
	};

	std::vector<LinkInfo> outlinks = m_pageDataCollector->outlinks();
	outlinks = PageParserHelpers::resolveUrlList(parsedPage->url, outlinks);

	handlePageLinkList(outlinks);

	m_uniqueLinkStore->saveLinkList(outlinks, RequestTypeGet);

	if (!parsedPage->redirectedUrl.isEmpty())
	{
		m_uniqueLinkStore->saveUrlList(std::vector<QUrl>{ parsedPage->redirectedUrl }, RequestTypeGet);
	}

	std::vector<QUrl> resourcesHeadUrlList;
	std::vector<QUrl> resourcesGetUrlList;

	for (const RawResourceOnPage& resource : parsedPage->allResourcesOnPage)
	{
		if (PageParserHelpers::isHttpOrHttpsScheme(resource.thisResourceLink.url) &&
			resource.resourceType != ResourceType::ResourceHtml)
		{
			if (resource.resourceType == ResourceType::ResourceImage)
			{
				resourcesGetUrlList.push_back(resource.thisResourceLink.url);
			}
			else
			{
				resourcesHeadUrlList.push_back(resource.thisResourceLink.url);
			}
		}
	}

	m_uniqueLinkStore->saveUrlList(resourcesGetUrlList, RequestTypeGet);
	m_uniqueLinkStore->saveUrlList(resourcesHeadUrlList, RequestTypeHead);
}

void CrawlerWorkerThread::handlePageLinkList(std::vector<LinkInfo>& linkList) const
{
	const auto isNofollowLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get()](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo) == OptionsLinkFilter::PermissionNofollowNotAllowed;
	};

	const auto isLinkBlockedByRobotsTxt = [optionsLinkFilter = m_optionsLinkFilter.get()](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo) == OptionsLinkFilter::PermissionBlockedByRobotsTxtRules;
	};

	const auto isSubdomainLinkUnavailable = [optionsLinkFilter = m_optionsLinkFilter.get()](const LinkInfo& linkInfo)
	{
		return optionsLinkFilter->linkPermission(linkInfo) == OptionsLinkFilter::PermissionSubdomainNotAllowed;
	};

	const auto removeUrlLastSlashIfExists = [](LinkInfo& link)
	{
		PageParserHelpers::removeUrlLastSlashIfExists(link.url);
	};

	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isNofollowLinkUnavailable), linkList.end());
	linkList.erase(std::remove_if(linkList.begin(), linkList.end(), isSubdomainLinkUnavailable), linkList.end());

	const auto emitPageParsedForBlockedPages = [this](const LinkInfo& linkInfo)
	{
		ParsedPagePtr page(new ParsedPage);

		page->url = linkInfo.url;
		page->title = tr("Blocked by robots.txt rules");

		//
		// TODO: change it on ResourceType::ResourceUnknown
		// It adding new type requires rework of ModelController
		//
		page->resourceType = ResourceType::ResourceHtml;

		emit pageParsed(page);
	};

	const auto blockedByRobotsTxtLinksIterator = std::remove_if(linkList.begin(), linkList.end(), isLinkBlockedByRobotsTxt);
	std::for_each(blockedByRobotsTxtLinksIterator, linkList.end(), emitPageParsedForBlockedPages);
	linkList.erase(blockedByRobotsTxtLinksIterator, linkList.end());

	std::for_each(linkList.begin(), linkList.end(), removeUrlLastSlashIfExists);
}

}

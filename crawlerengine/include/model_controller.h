#pragma once

#include "parsed_page.h"
#include "crawler_options.h"
#include "sequenced_data_collection_types.h"
#include "storage_type.h"
#include "worker_result.h"

namespace CrawlerEngine
{

class UnorderedDataCollection;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController();

	const UnorderedDataCollection* data() const noexcept;
	UnorderedDataCollection* data() noexcept;

	static bool resourceShouldBeProcessed(ResourceType resourceType, const CrawlerOptionsData& optionsData) noexcept;

signals:
	void refreshPageDone();

public slots:
	void handleWorkerResult(CrawlerEngine::WorkerResult workerResult) noexcept;
	void setWebCrawlerOptions(const CrawlerEngine::CrawlerOptionsData& optionsData);
	void clearData();
	void preparePageForRefresh(CrawlerEngine::ParsedPage* parsedPage, int turnaround);

private:
	void processParsedPageUrl(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageTitle(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageMetaDescription(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageMetaKeywords(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageH1(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageH2(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageImage(WorkerResult& workerResult, bool checkOnlyLastResource = false, bool secondGetRequest = false);
	void processParsedPageStatusCode(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageHtmlResources(WorkerResult& workerResult, bool secondGetRequest);
	void processParsedPageResources(WorkerResult& workerResult, bool secondGetRequest);
	void fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept;
	void removeResourceFromPendingStorageIfNeeded(ParsedPagePtr& incomingPage, int turnaround) noexcept;
	void calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept;
	void setPageLevel(ParsedPagePtr& page, int level) const noexcept;
	void addDuplicates(ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage, int turnaround, bool checkCanonicals = true);

	ParsedPagePtr parsedPageFromResource(const ResourceOnPage& resource) const;
	QSet<StorageType> addIndexingBlockingPage(ParsedPagePtr& pageFromResource, bool existingResource, const ResourceOnPage& resource, bool isParentResourceBlockedByMetaRobot, int turnaround);
	void removeFromIndexingBlockingPagesIfNeeded(ParsedPagePtr& existingResource, int turnaround);

	void addToBlockedByXrobotsTagIfNeeded(WorkerResult& workerResult);

	std::pair<ParsedPagePtr, StorageType> tryGetPageFromCrawledOrPendingStorage(const ParsedPagePtr& pointer) const;
	ParsedPagePtr takeFromCrawledOrPendingStorage(const ParsedPagePtr& pointer) const;
	void setLinksForResourcePageAndLoadedPage(ParsedPagePtr& resourcePage, WorkerResult& loadedPage, const ResourceOnPage& resource) const;

	void handlePresenceYandexMetricaCounters(WorkerResult& workerResult, bool secondGetRequest);
	void handlePresenceGoogleAnalyticsCounters(WorkerResult& workerResult, bool secondGetRequest);

private:
	UnorderedDataCollection* m_data;
	CrawlerOptionsData m_crawlerOptionsData;
	LinksToThisResourceChanges m_linksToPageChanges;
};

}

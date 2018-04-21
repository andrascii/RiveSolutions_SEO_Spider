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
	void handleWorkerResult(WorkerResult workerResult) noexcept;
	void setWebCrawlerOptions(const CrawlerOptionsData& optionsData);
	void clearData();
	void preparePageForRefresh(ParsedPage* parsedPage);

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
	void removeResourceFromPendingStorageIfNeeded(ParsedPagePtr& incomingPage) noexcept;
	void calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept;
	void setPageLevel(ParsedPagePtr& page, int level) const noexcept;
	void addDuplicates(ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage, bool checkCanonicals = true);

	ParsedPagePtr parsedPageFromResource(const ResourceOnPage& resource) const;
	QSet<StorageType> addIndexingBlockingPage(ParsedPagePtr& pageFromResource, const ResourceOnPage& resource);

private:
	UnorderedDataCollection* m_data;
	CrawlerOptionsData m_crawlerOptionsData;
	LinksToThisResourceChanges m_linksToPageChanges;
};

}

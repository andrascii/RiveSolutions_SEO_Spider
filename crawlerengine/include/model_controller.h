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

	const UnorderedDataCollection* data() const noexcept
	{
		return m_data;
	}

	UnorderedDataCollection* data() noexcept
	{
		return m_data;
	}

public slots:
	void handleWorkerResult(WorkerResult workerResult) noexcept;
	void setWebCrawlerOptions(const CrawlerOptions& options);
	void clearData();
	void preparePageForRefresh(ParsedPage* parsedPage);

private:
	void processParsedPageUrl(WorkerResult& workerResult);
	void processParsedPageTitle(WorkerResult& workerResult);
	void processParsedPageMetaDescription(WorkerResult& workerResult);
	void processParsedPageMetaKeywords(WorkerResult& workerResult);
	void processParsedPageH1(WorkerResult& workerResult);
	void processParsedPageH2(WorkerResult& workerResult);
	void processParsedPageImage(WorkerResult& workerResult, bool checkOnlyLastResource = false);
	void processParsedPageStatusCode(WorkerResult& workerResult);
	void processParsedPageHtmlResources(WorkerResult& workerResult);
	void processParsedPageResources(WorkerResult& workerResult);
	void fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept;
	bool resourceShouldBeProcessed(ResourceType resourceType) const noexcept;
	void calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept;
	void setPageLevel(ParsedPagePtr& page, int level) const noexcept;
	void addDuplicates(ParsedPagePtr& incomingPage, StorageType lookupStorage, StorageType destStorage);

private:
	UnorderedDataCollection* m_data;
	CrawlerOptions m_crawlerOptions;
	LinksToThisResourceChanges m_linksToPageChanges;
};

}

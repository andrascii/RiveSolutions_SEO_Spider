#pragma once

#include "parsed_page.h"
#include "parsed_page_hasher_proxy.h"
#include "parsed_page_comparator.h"
#include "storage_type.h"
#include "sequenced_data_collection_types.h"
#include "worker_result.h"
#include "icustom_data_feed.h"

namespace CrawlerEngine
{

class ISequencedStorage;
class UnorderedDataCollection;

class SequencedDataCollection : public QObject
{
	Q_OBJECT

public:
	SequencedDataCollection(const UnorderedDataCollection* collection);

	void initialize();
	bool empty() const noexcept;

	const ISequencedStorage* storage(StorageType type) const noexcept;
	ISequencedStorage* storage(StorageType type) noexcept;
	bool removePage(ParsedPage* parsedPage, StorageType type);
	void setCustomDataFeeds(const QVector<ICustomDataFeed*> dataFeeds);
	const QVector<ICustomDataFeed*>& customDataFeeds() const;

signals:
	void parsedPageAdded(int row, CrawlerEngine::StorageType type);
	void parsedPageReplaced(int row, CrawlerEngine::StorageType type);
	void parsedPageRemoved(int row, CrawlerEngine::StorageType type);
	void parsedPagesRemoved(int count, CrawlerEngine::StorageType type);
	void indicesRangeInvalidated(std::pair<int, int> indicesRange, CrawlerEngine::StorageType type);
	void parsedPageLinksToThisResourceChanged(CrawlerEngine::LinksToThisResourceChanges changes);
	void beginClearData();
	void endClearData();

protected:
	virtual std::shared_ptr<ISequencedStorage> createSequencedStorage() const;

protected slots:
	void addParsedPage(CrawlerEngine::ParsedPagePtr parsedPagePtr, CrawlerEngine::StorageType type, int turnaround);
	void addParsedPage(CrawlerEngine::WorkerResult workerResult, CrawlerEngine::StorageType type);
	void replaceParsedPage(CrawlerEngine::ParsedPagePtr oldParsedPagePtr, CrawlerEngine::ParsedPagePtr newParsedPagePtr, CrawlerEngine::StorageType type, int turnaround);
	void onParsedPageRemoved(CrawlerEngine::ParsedPagePtr parsedPagePointer, CrawlerEngine::StorageType type, int turnaround);
	void onDataCleared();
	void onCustomDataFeedRowReceived(CrawlerEngine::ICustomDataFeedRow* row);

private:
	std::unordered_map<StorageType, std::shared_ptr<ISequencedStorage>> m_sequencedStorageMap;
	QVector<ICustomDataFeed*> m_customDataFeeds;
};

}

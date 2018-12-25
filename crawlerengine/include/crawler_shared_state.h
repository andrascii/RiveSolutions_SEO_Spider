#pragma once

namespace CrawlerEngine
{

class CrawlerSharedState
{
public:
	int downloaderCrawledLinksCount() const noexcept;
	void incrementDownloaderCrawledLinksCount() noexcept;
	void setDownloaderCrawledLinksCount(int count) noexcept;

	int downloaderPendingLinksCount() const noexcept;
	void incrementDownloaderPendingLinksCount() noexcept;
	void decrementDownloaderPendingLinksCount() noexcept;
	void setDownloaderPendingLinksCount(int count) noexcept;

	int workersProcessedLinksCount() const noexcept;
	void incrementWorkersProcessedLinksCount() noexcept;
	void setWorkersProcessedLinksCount(int count) noexcept;

	int modelControllerAcceptedLinksCount() const noexcept;
	void incrementModelControllerAcceptedLinksCount() noexcept;
	void setModelControllerAcceptedLinksCount(int count) noexcept;

	int modelControllerCrawledLinksCount() const noexcept;
	void incrementModelControllerCrawledLinksCount() noexcept;
	void setModelControllerCrawledLinksCount(int count) noexcept;

	int sequencedDataCollectionLinksCount() const noexcept;
	void incrementSequencedDataCollectionLinksCount() noexcept;
	void setSequencedDataCollectionLinksCount(int count) noexcept;

	int turnaround() const noexcept;

	void clear() noexcept;

	int additionalPendingCount() const noexcept;

	static CrawlerSharedState* instance() noexcept;

private:
	CrawlerSharedState();
	void incrementTurnaround() noexcept;

private:
	std::atomic<int> m_downloaderCrawledLinksCount;
	std::atomic<int> m_downloaderPendingLinksCount;
	std::atomic<int> m_workersProcessedLinksCount;
	std::atomic<int> m_modelControllerAcceptedLinksCount;
	std::atomic<int> m_modelControllerCrawledLinksCount;
	std::atomic<int> m_sequencedDataCollectionLinksCount;
	std::atomic<int> m_turnaround;
};

}
#include "crawler_shared_state.h"

namespace CrawlerEngine
{

CrawlerSharedState::CrawlerSharedState()
	: m_downloaderCrawledLinksCount(0)
	, m_downloaderPendingLinksCount(0)
	, m_workersProcessedLinksCount(0)
	, m_modelControllerAcceptedLinksCount(0)
	, m_modelControllerCrawledLinksCount(0)
	, m_sequencedDataCollectionLinksCount(0)
{
}

int CrawlerSharedState::downloaderCrawledLinksCount() const noexcept
{
	return m_downloaderCrawledLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementDownloaderCrawledLinksCount() noexcept
{
	m_downloaderCrawledLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::setDownloaderCrawledLinksCount(int count) noexcept
{
	m_downloaderCrawledLinksCount.store(count, std::memory_order_release);
}

int CrawlerSharedState::downloaderPendingLinksCount() const noexcept
{
	return m_downloaderPendingLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementDownloaderPendingLinksCount() noexcept
{
	m_downloaderPendingLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::decrementDownloaderPendingLinksCount() noexcept
{
	m_downloaderPendingLinksCount.fetch_sub(1, std::memory_order_release);
}

void CrawlerSharedState::setDownloaderPendingLinksCount(int count) noexcept
{
	m_downloaderPendingLinksCount.store(count, std::memory_order_release);
}

int CrawlerSharedState::workersProcessedLinksCount() const noexcept
{
	return m_workersProcessedLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementWorkersProcessedLinksCount() noexcept
{
	m_workersProcessedLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::setWorkersProcessedLinksCount(int count) noexcept
{
	m_workersProcessedLinksCount.store(count, std::memory_order_release);
}

int CrawlerSharedState::modelControllerAcceptedLinksCount() const noexcept
{
	return m_modelControllerAcceptedLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementModelControllerAcceptedLinksCount() noexcept
{
	m_modelControllerAcceptedLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::setModelControllerAcceptedLinksCount(int count) noexcept
{
	m_modelControllerAcceptedLinksCount.store(count, std::memory_order_release);
}

int CrawlerSharedState::modelControllerCrawledLinksCount() const noexcept
{
	return m_modelControllerCrawledLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementModelControllerCrawledLinksCount() noexcept
{
	m_modelControllerCrawledLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::setModelControllerCrawledLinksCount(int count) noexcept
{
	m_modelControllerCrawledLinksCount.store(count, std::memory_order_release);
}

int CrawlerSharedState::sequencedDataCollectionLinksCount() const noexcept
{
	return m_sequencedDataCollectionLinksCount.load(std::memory_order_acquire);
}

void CrawlerSharedState::incrementSequencedDataCollectionLinksCount() noexcept
{
	m_sequencedDataCollectionLinksCount.fetch_add(1, std::memory_order_release);
}

void CrawlerSharedState::setSequencedDataCollectionLinksCount(int count) noexcept
{
	m_sequencedDataCollectionLinksCount.store(count, std::memory_order_release);
}

void CrawlerSharedState::clear() noexcept
{
	m_downloaderCrawledLinksCount.store(0, std::memory_order_release);
	m_downloaderPendingLinksCount.store(0, std::memory_order_release);
	m_workersProcessedLinksCount.store(0, std::memory_order_release);
	m_modelControllerAcceptedLinksCount.store(0, std::memory_order_release);
	m_modelControllerCrawledLinksCount.store(0, std::memory_order_release);
	m_sequencedDataCollectionLinksCount.store(0, std::memory_order_release);
}

CrawlerSharedState* CrawlerSharedState::instance() noexcept
{
	static CrawlerSharedState s_instance;
	return &s_instance;
}

}

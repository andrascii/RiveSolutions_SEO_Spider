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
	return m_downloaderCrawledLinksCount;
}

void CrawlerSharedState::incrementDownloaderCrawledLinksCount() noexcept
{
	++m_downloaderCrawledLinksCount;
}

void CrawlerSharedState::setDownloaderCrawledLinksCount(int count) noexcept
{
	m_downloaderCrawledLinksCount.store(count);
}

int CrawlerSharedState::downloaderPendingLinksCount() const noexcept
{
	return m_downloaderPendingLinksCount;
}

void CrawlerSharedState::incrementDownloaderPendingLinksCount() noexcept
{
	++m_downloaderPendingLinksCount;
}

void CrawlerSharedState::decrementDownloaderPendingLinksCount() noexcept
{
	--m_downloaderPendingLinksCount;
}

void CrawlerSharedState::setDownloaderPendingLinksCount(int count) noexcept
{
	m_downloaderPendingLinksCount.store(count);
}

int CrawlerSharedState::workersProcessedLinksCount() const noexcept
{
	return m_workersProcessedLinksCount;
}

void CrawlerSharedState::incrementWorkersProcessedLinksCount() noexcept
{
	++m_workersProcessedLinksCount;
}

void CrawlerSharedState::setWorkersProcessedLinksCount(int count) noexcept
{
	m_workersProcessedLinksCount.store(count);
}

int CrawlerSharedState::modelControllerAcceptedLinksCount() const noexcept
{
	return m_modelControllerAcceptedLinksCount;
}

void CrawlerSharedState::incrementModelControllerAcceptedLinksCount() noexcept
{
	++m_modelControllerAcceptedLinksCount;
}

void CrawlerSharedState::setModelControllerAcceptedLinksCount(int count) noexcept
{
	m_modelControllerAcceptedLinksCount.store(count);
}

int CrawlerSharedState::modelControllerCrawledLinksCount() const noexcept
{
	return m_modelControllerCrawledLinksCount;
}

void CrawlerSharedState::incrementModelControllerCrawledLinksCount() noexcept
{
	++m_modelControllerCrawledLinksCount;
}

void CrawlerSharedState::setModelControllerCrawledLinksCount(int count) noexcept
{
	m_modelControllerCrawledLinksCount.store(count);
}

int CrawlerSharedState::sequencedDataCollectionLinksCount() const noexcept
{
	return m_sequencedDataCollectionLinksCount;
}

void CrawlerSharedState::incrementSequencedDataCollectionLinksCount() noexcept
{
	++m_sequencedDataCollectionLinksCount;
}

void CrawlerSharedState::setSequencedDataCollectionLinksCount(int count) noexcept
{
	m_sequencedDataCollectionLinksCount.store(count);
}

void CrawlerSharedState::clear() noexcept
{
	m_downloaderCrawledLinksCount.store(0);
	m_downloaderPendingLinksCount.store(0);
	m_workersProcessedLinksCount.store(0);
	m_modelControllerAcceptedLinksCount.store(0);
	m_modelControllerCrawledLinksCount.store(0);
	m_sequencedDataCollectionLinksCount.store(0);
}

CrawlerSharedState * CrawlerSharedState::instance() noexcept
{
	static CrawlerSharedState s_instance;
	return &s_instance;
}

}

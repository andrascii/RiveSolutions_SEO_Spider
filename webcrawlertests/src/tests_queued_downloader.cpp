#include "tests_queued_downloader.h"

namespace WebCrawlerTests
{
	
void TestsQueudedDownoader::scheduleUrl(const WebCrawler::CrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);
	Reply reply;
	reply.statusCode = 200;
	reply.url = url.url;

	m_repliesQueue.push_back(reply);
}

bool TestsQueudedDownoader::extractReply(Reply& response) noexcept
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	if (m_repliesQueue.empty())
	{
		return false;
	}

	response = std::move(*m_repliesQueue.begin());
	m_repliesQueue.erase(m_repliesQueue.begin());

	return true;
}

void TestsQueudedDownoader::start() noexcept
{
	// do nothing
}

void TestsQueudedDownoader::stop() noexcept
{
	// do nothing
}

}
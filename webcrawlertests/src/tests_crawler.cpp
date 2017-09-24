#include "tests_crawler.h"
#include "tests_queued_downloader.h"

namespace WebCrawlerTests
{

TestsCrawler::TestsCrawler(unsigned int threadCount)
	: WebCrawler::Crawler(threadCount)
{

}

WebCrawler::IQueuedDownloader* TestsCrawler::createQueuedDownloader() const noexcept
{
	return new TestsQueudedDownoader();
}

}
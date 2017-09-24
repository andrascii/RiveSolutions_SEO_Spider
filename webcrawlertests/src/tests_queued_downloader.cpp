#include "tests_queued_downloader.h"

namespace WebCrawlerTests
{
	
void TestsQueudedDownoader::scheduleUrl(const WebCrawler::CrawlerRequest& url) noexcept
{
	Q_UNUSED(url);
	// TODO: implement
}

bool TestsQueudedDownoader::extractReply(Reply& response) noexcept
{
	Q_UNUSED(response);
	// TODO: implement
	return false;
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
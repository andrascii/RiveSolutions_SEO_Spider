#pragma once
#include "iqueued_dowloader.h"

namespace WebCrawlerTests
{
	
class TestsQueudedDownoader : public WebCrawler::IQueuedDownloader
{
	virtual void scheduleUrl(const WebCrawler::CrawlerRequest& url) noexcept override;

	virtual bool extractReply(Reply& response) noexcept override;

	virtual void start() noexcept override;
	virtual void stop() noexcept override;
};

}
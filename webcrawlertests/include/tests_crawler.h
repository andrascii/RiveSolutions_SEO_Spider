#pragma once
#include "crawler.h"

namespace WebCrawlerTests
{

class TestsCrawler : public WebCrawler::Crawler
{
public:
	TestsCrawler(unsigned int threadCount);

protected:
	virtual WebCrawler::IQueuedDownloader* createQueuedDownloader() const noexcept override;
};

}
#pragma once

#include "web_crawler.h"
#include "page_raw.h"
#include "queued_downloader.h"
#include "abstract_threadable_object.h"
#include "html_page_parser.h"

namespace WebCrawler
{			

class WebCrawlerInternalUrlStorage;

class PageRawProcessor : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	PageRawProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, 
		QueuedDownloader* queuedDownloader, QObject* parent = nullptr);

	Q_SIGNAL void webPageParsed(PageRawPtr pageRaw);

private:
	virtual void process() override;

private:
	HtmlPageParser m_htmlPageParser;
	WebCrawlerInternalUrlStorage* m_webCrawlerInternalUrlStorage;
	QueuedDownloader* m_queuedDownloader;
	PageRawPtr m_pageRaw;
	std::vector<QUrl> m_pageUrlList;
};

}

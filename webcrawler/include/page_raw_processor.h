#pragma once

#include "web_crawler.h"
#include "page_raw.h"
#include "queued_downloader.h"
#include "abstract_threadable_object.h"
#include "html_page_parser.h"

namespace WebCrawler
{			

class CrawlerUrlStorage;

class PageRawProcessor : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	PageRawProcessor(CrawlerUrlStorage* crawlerStorage, 
		QueuedDownloader* queuedDownloader, QObject* parent = nullptr);

	Q_SIGNAL void webPageParsed(PageRawPtr pageRaw);
	Q_SLOT void setHost(QUrl host);

private:
	virtual void process() override;

private:
	HtmlPageParser m_htmlPageParser;
	CrawlerUrlStorage* m_webCrawlerInternalUrlStorage;
	QueuedDownloader* m_queuedDownloader;
	std::vector<QUrl> m_pageUrlList;
	QUrl m_host;
};

}

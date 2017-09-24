#pragma once

#include "crawler.h"
#include "parsed_page.h"
#include "iqueued_dowloader.h"
#include "abstract_threadable_object.h"
#include "html_page_parser.h"

namespace WebCrawler
{			

class CrawlerUrlStorage;

class CrawlerWorkerThread : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	CrawlerWorkerThread(
		CrawlerUrlStorage* crawlerStorage, 
		IQueuedDownloader* queuedDownloader, 
		QObject* parent = nullptr
	);

	Q_SIGNAL void pageParsed(ParsedPagePtr pageRaw);
	Q_SLOT void setHost(QUrl host);

private:
	virtual void process() override;

	void preprocessRedirect(const ParsedPagePtr& pageRaw, const QUrl& redirectUrl);
	void schedulePageResourcesLoading(const ParsedPagePtr& pageRaw);

private:
	HtmlPageParser m_htmlPageParser;
	CrawlerUrlStorage* m_webCrawlerInternalUrlStorage;
	IQueuedDownloader* m_queuedDownloader;
	QUrl m_host;
};

}

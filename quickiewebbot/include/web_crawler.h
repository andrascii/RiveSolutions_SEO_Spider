#pragma once

#include "page_info.h"
#include "page_info_acceptor.h"
#include "web_crawler_internal_url_storage.h"
#include "queued_downloader.h"

namespace QuickieWebBot
{

class PageInfoAcceptor;
class ModelController;

class WebCrawler : public QObject
{
	Q_OBJECT

public:
	WebCrawler(unsigned int threadCount, ModelController* modelController, QObject* parent = nullptr);
	~WebCrawler();

	Q_SLOT void startCrawling();
	Q_SLOT void stopCrawling();

private:
	Q_SLOT void onPageInfoParsed(PageInfoPtr pageInfo);

private:
	QueuedDownloader m_queuedDownloader;

	ModelController* m_modelController;
	
	WebCrawlerInternalUrlStorage m_internalUrlStorage;

	std::vector<std::unique_ptr<PageInfoAcceptor>> m_workers;
};

}

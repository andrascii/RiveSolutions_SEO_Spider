#pragma once

#include "web_crawler.h"
#include "page_info.h"
#include "gumbo.h"
#include "queued_downloader.h"
#include "abstract_threadable_object.h"
#include "html_page_parser.h"

namespace QuickieWebBot
{			

class WebCrawlerInternalUrlStorage;

class PageInfoProcessor : public AbstractThreadableObject
{			
	Q_OBJECT
			
public:
	PageInfoProcessor(WebCrawlerInternalUrlStorage* crawlerStorage, 
		QueuedDownloader* queuedDownloader, QObject* parent = nullptr);

	Q_SIGNAL void webPageParsed(PageInfoPtr pageInfo);

private:
	virtual void process() override;

	void validateUrlList(std::vector<QUrl>& urlList) noexcept;

private:
	HtmlPageParser m_htmlPageParser;

	WebCrawlerInternalUrlStorage* m_webCrawlerInternalUrlStorage;

	QueuedDownloader* m_queuedDownloader;

	PageInfoPtr m_pageInfo;

	std::vector<QUrl> m_pageUrlList;
};

}

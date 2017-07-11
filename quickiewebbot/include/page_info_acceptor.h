#pragma once

#include "web_crawler.h"
#include "page_info.h"
#include "gumbo.h"
#include "queued_downloader.h"

namespace QuickieWebBot
{			

class WebCrawlerInternalUrlStorage;

class PageInfoAcceptor : public QObject
{			
	Q_OBJECT
			
public:
	PageInfoAcceptor(WebCrawlerInternalUrlStorage* crawlerStorage, 
		QueuedDownloader* queuedDownloader, QObject* parent = nullptr);

	const std::vector<QUrl>& pageUrlList() const noexcept;

	Q_INVOKABLE void start();
	Q_INVOKABLE void stop();
	
	Q_SIGNAL void pageParsed(PageInfoPtr pageInfo);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void processWebPage();

	void parseWebPage(const QString& htmlPage) noexcept;
	void parseWebPageUrlList(const GumboNode* node) noexcept;
	void parseWebPageTitle(const GumboNode* head) noexcept;
	void parseWebPageMeta(const GumboNode* head) noexcept;

	GumboNode* firstSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent = 0) const noexcept;
	std::vector<GumboNode*> allSubNodes(const GumboNode* node, GumboTag tag) const noexcept;
	QString nodeText(const GumboNode* node) const noexcept;
	unsigned countChildren(const GumboNode* node, GumboTag tag) const noexcept;

private:
	WebCrawlerInternalUrlStorage* m_webCrawlerInternalUrlStorage;

	QueuedDownloader* m_queuedDownloader;

	PageInfoPtr m_pageInfo;

	int m_timerId;

	std::vector<QUrl> m_pageUrlList;
};

}

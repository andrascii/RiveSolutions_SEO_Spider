#pragma once

#include "crawler.h"
#include "page_info.h"
#include "gumbo.h"

namespace QuickieWebBot
{			

class NetworkAccessManagerFutureProvider;
class CrawlerStorage;

class PageInfoAcceptor : public QObject
{			
	Q_OBJECT
			
public:
	PageInfoAcceptor(CrawlerStorage* crawlerStorage, QObject* parent = nullptr);

	const std::vector<QUrl>& pageUrlList() const noexcept;

	Q_INVOKABLE void start();
	Q_SIGNAL void pageParsed(PageInfoPtr pageInfo);

private:
	void parsePage(const QString& htmlPage) noexcept;
	void parsePageUrlList(const GumboNode* node) noexcept;
	void parsePageTitle(const GumboNode* head) noexcept;
	void parsePageMeta(const GumboNode* head) noexcept;

	GumboNode* firstSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent = 0) const noexcept;
	std::vector<GumboNode*> allSubNodes(const GumboNode* node, GumboTag tag) const noexcept;
	QString nodeText(const GumboNode* node) const noexcept;
	unsigned countChildren(const GumboNode* node, GumboTag tag) const noexcept;

private:
	CrawlerStorage* m_crawlerStorage;
	std::atomic_bool m_stop;
	PageInfoPtr m_pageInfo;
	std::vector<QUrl> m_pageUrlList;
};

}

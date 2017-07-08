#pragma once

#include "page_info.h"
#include "gumbo.h"

namespace QuickieWebBot
{			
			
class CrawlerPageInfoAcceptor : public QObject
{			
	Q_OBJECT
			
public:		
	enum State
	{
		WorkingState,
		PendingState
	};

	CrawlerPageInfoAcceptor(QObject* parent = nullptr);
			
	const std::vector<QUrl>& pageUrlList() const noexcept;
	Q_INVOKABLE void handlePage(QUrl url);
			
	Q_SIGNAL void pageParsed(QThread* fromThread, PageInfoPtr pageInfo);
	Q_SIGNAL void urlListParsed(const std::vector<QUrl>& urls);
			
private:	
	Q_SLOT void pageDownloaded(QNetworkReply* reply);
			
	void parsePageUrlList(const GumboNode* node) noexcept;
	void parsePage(const QString& htmlPage) noexcept;
	void parsePageTitle(const GumboNode* head) noexcept;
	void parsePageMeta(const GumboNode* head) noexcept;

	GumboNode* firstSubNode(const GumboNode* node, GumboTag tag, unsigned startIndexWhithinParent = 0) const noexcept;
	std::vector<GumboNode*> allSubNodes(const GumboNode* node, GumboTag tag) const noexcept;
	QString nodeText(const GumboNode* node) const noexcept;
	unsigned countChildren(const GumboNode* node, GumboTag tag) const noexcept;

private:
	QNetworkAccessManager* m_networkAccesManager;
	PageInfoPtr m_pageInfo;
	std::atomic_int m_state;
	std::vector<QUrl> m_pageUrlList;
};

}

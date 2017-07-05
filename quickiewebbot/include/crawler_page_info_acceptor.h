#pragma once

#include "website_analyse_element.h"
#include "gumbo.h"

namespace QuickieWebBot
{

class CrawlerPageInfoAcceptor : public QObject
{
	Q_OBJECT

public:
	CrawlerPageInfoAcceptor(QObject* parent = nullptr);

	Q_SIGNAL void addElement(QThread* fromThread, WebSiteAnalyseElementPtr element);

	Q_SLOT void handleUrl(QUrl url);

private:
	void parsePage(QString const& htmlPage) noexcept;
	void parseTag(GumboNode* root, int gumboTagType) noexcept;

	void parseContent(GumboNode* head) noexcept;
	void parseMetaRefresh(GumboNode* head) noexcept;
	void parseMetaRobots(GumboNode* head) noexcept;
	void parseRedirectedUrl(GumboNode* head) noexcept;
	void parseTitle(GumboNode* head) noexcept;
	void parseMetaDescription(GumboNode* head) noexcept;
	void parseMetaKeywords(GumboNode* head) noexcept;
	void parseFirstH1(GumboNode* body) noexcept;
	void parseSecondH1(GumboNode* body) noexcept;
	void parseFirstH2(GumboNode* body) noexcept;
	void parseSecondH2(GumboNode* body) noexcept;

	Q_SLOT void pageDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccesManager;
	WebSiteAnalyseElementPtr m_element;
};

}

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
	void parseContent(GumboVector* head) noexcept;
	void parseMetaRefresh(GumboVector* head) noexcept;
	void parseMetaRobots(GumboVector* head) noexcept;
	void parseRedirectedUrl(GumboVector* head) noexcept;
	void parseTitle(GumboVector* head) noexcept;
	void parseMetaDescription(GumboVector* head) noexcept;
	void parseMetaKeywords(GumboVector* head) noexcept;
	void parseFirstH1(GumboVector* body) noexcept;
	void parseSecondH1(GumboVector* body) noexcept;
	void parseFirstH2(GumboVector* body) noexcept;
	void parseSecondH2(GumboVector* body) noexcept;

	Q_SLOT void pageDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccesManager;
	WebSiteAnalyseElementPtr m_element;
};

}

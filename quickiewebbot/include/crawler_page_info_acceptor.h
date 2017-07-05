#pragma once

#include "website_analyse_element.h"

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
	void parsePage();

	Q_SLOT void pageDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccesManager;
};

}

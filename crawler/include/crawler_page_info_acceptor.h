#pragma once

namespace QuickieWebBot
{

class CrawlerPageInfoAcceptor : public QObject
{
	Q_OBJECT

public:
	CrawlerPageInfoAcceptor(QObject* parent = nullptr);

	Q_INVOKABLE void downloadPage(QUrl url);

private:
	void parsePage();

	Q_SLOT void pageDownloaded(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccesManager;
};

}

#pragma once
#include "irobots_txt_loader.h"

namespace WebCrawler
{

class RobotsTxtLoader : public QObject, public IRobotsTxtLoader
{
	Q_OBJECT

public:
	RobotsTxtLoader(QNetworkAccessManager* networkAccessor);

	virtual void load(const QUrl& url) override;
	Q_SIGNAL virtual void ready(const QByteArray& content) override;
	virtual QObject* qObject() override;

private:

	Q_SLOT void onLoadingDone(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccessor;
};

}

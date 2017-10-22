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
	virtual const QByteArray& content() const noexcept override;
	virtual bool isReady() const noexcept override;

	virtual QObject* qobject() override;

signals:
	virtual void ready() override;

private slots:
	void onLoadingDone(QNetworkReply* reply);

private:
	bool m_isReady;
	QByteArray m_content;
	QNetworkAccessManager* m_networkAccessor;
};

}

#pragma once

#include "irobots_txt_loader.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

struct DownloadResponse;

class RobotsTxtLoader : public QObject, public IRobotsTxtLoader
{
	Q_OBJECT

public:
	RobotsTxtLoader(QObject* parent = nullptr);

	virtual void setUserAgent(const QByteArray& userAgent) override;

	virtual void load(const QUrl& url) override;
	virtual const QByteArray& content() const noexcept override;
	virtual bool isReady() const noexcept override;

	virtual QObject* qobject() override;

signals:
	virtual void ready() override;

private slots:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

private:
	bool m_isReady;
	QByteArray m_content;
	QByteArray m_userAgent;
	QUrl m_currentLoadedUrl;
	RequesterWrapper m_downloadRequester;
};

}

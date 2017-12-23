#pragma once

#include "ispecific_loader.h"
#include "custom_url.h"
#include "requester_wrapper.h"

namespace CrawlerEngine
{

struct DownloadResponse;
class RobotsTxtLoader;
class Requester;

class XmlSitemapLoader : public QObject, public ISpecificLoader
{
	Q_OBJECT

public:
	XmlSitemapLoader(RobotsTxtLoader* robotsTxtLoader, QObject* parent = nullptr);

	virtual void load(const CustomUrl& host) override;
	virtual const QByteArray& content() const noexcept override;
	virtual bool isReady() const noexcept override;
	virtual bool isValid() const noexcept override;
	virtual QObject* qobject() override;

signals:
	virtual void ready() override;

private slots:
	void onRobotsTxtLoaderReady();

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

private:
	bool m_isReady;
	bool m_isValid;
	QByteArray m_content;
	CustomUrl m_currentLoadedUrl;
	RequesterWrapper m_downloadRequester;
	RobotsTxtLoader* m_robotsTxtLoader;
};

}
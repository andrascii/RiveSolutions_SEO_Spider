#pragma once

#include "ispecific_loader.h"
#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{

struct DownloadResponse;

class RobotsTxtLoader : public QObject, public ISpecificLoader
{
	Q_OBJECT

public:
	RobotsTxtLoader(QObject* parent = nullptr);

	virtual void load(const Url& host) override;
	virtual const QByteArray& content() const noexcept override;
	virtual bool isReady() const noexcept override;
	virtual bool isValid() const noexcept override;
	virtual QObject* qobject() override;

signals:
	virtual void ready() override;

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);

private:
	bool m_isReady;
	bool m_isValid;
	QByteArray m_content;
	Url m_currentLoadedUrl;
	RequesterWrapper m_downloadRequester;
};

}

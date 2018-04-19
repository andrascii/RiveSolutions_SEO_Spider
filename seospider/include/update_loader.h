#pragma once

#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{

struct DownloadResponse;
struct DownloadProgressResponse;
class Requester;

}

namespace SeoSpider
{

class UpdateLoader : public QObject
{
	Q_OBJECT

public:
	UpdateLoader(QObject* parent = nullptr);

	void startDownloadingUpdate(const CrawlerEngine::Url& url);
	void stopDownloading();

signals:
	void downloadProgress(quint64 bytesReceived, quint64 bytesTotal, double downloadSpeed);
	void updateDownloaded(const QString& filepath);

private:
	void onAboutDownloadProgress(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadProgressResponse& response);
	void onUpdatesDownloadingFinished(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);

private:
	CrawlerEngine::RequesterWrapper m_downloadRequester;
	QTime m_downloadTime;
	CrawlerEngine::Url m_url;
};

}
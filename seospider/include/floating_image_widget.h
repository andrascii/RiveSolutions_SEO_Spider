#pragma once

#include "requester_wrapper.h"

namespace CrawlerEngine
{
	struct DownloadResponse;
}

namespace SeoSpider
{

class FloatingImageWidget : public QLabel
{
public:
	FloatingImageWidget(QWidget* parent = nullptr);

	void setCurrentUrl(const QUrl& url);
	void invalidateUrl();

private slots:
	void onLoadingDone(CrawlerEngine::Requester* requester, const CrawlerEngine::DownloadResponse& response);

private:
	CrawlerEngine::RequesterWrapper m_requester;
	QUrl m_url;
	bool m_loadingDone;
};

}

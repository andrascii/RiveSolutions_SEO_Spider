#include "update_loader.h"
#include "download_request.h"
#include "download_response.h"
#include "update_helpers.h"
#include "application.h"

namespace SeoSpider
{

UpdateLoader::UpdateLoader(QObject* parent)
	: QObject(parent)
{
}

void UpdateLoader::startDownloadingUpdate(const CrawlerEngine::Url& url)
{
	if (m_downloadRequester.get())
	{
		WARNLOG << "Downloading already started";
		return;
	}

	m_url = url;

	CrawlerEngine::CrawlerRequest crawlerRequest
	{
		url,
		CrawlerEngine::DownloadRequestType::RequestTypeGet
	};

	CrawlerEngine::DownloadRequest request(
		crawlerRequest,
		CrawlerEngine::DownloadRequest::LinkStatus::LinkStatusFirstLoading,
		CrawlerEngine::DownloadRequest::BodyProcessingCommand::CommandDownloadBodyAnyway
	);

	m_downloadRequester.reset(request, this, &UpdateLoader::onAboutDownloadProgress, &UpdateLoader::onUpdatesDownloadingFinished);
	m_downloadRequester->start();
	m_downloadTime.start();
}

void UpdateLoader::stopDownloading()
{
	if (!m_downloadRequester.get())
	{
		return;
	}

	m_downloadRequester->stop();
	m_downloadTime = QTime();
}

void UpdateLoader::onAboutDownloadProgress(CrawlerEngine::Requester*, const CrawlerEngine::DownloadProgressResponse& response)
{
	emit downloadProgress(response.bytesReceived, response.bytesTotal, response.bytesReceived * 1000.0 / m_downloadTime.elapsed());
}

void UpdateLoader::onUpdatesDownloadingFinished(CrawlerEngine::Requester*, const CrawlerEngine::DownloadResponse& response)
{
	const QString filepath = UpdateHelpers::updatesPatchSaveDirPath() + "/" + m_url.fileName();

	QFile file(filepath);
	file.open(QIODevice::WriteOnly);

	DEBUG_ASSERT(response.hopsChain.length() > 0);

	if (response.hopsChain.length() == 0)
	{
		ERRLOG << "For some reason hops chain is empty";
		return;
	}

	const CrawlerEngine::Hop lastHop = response.hopsChain.back();

	file.write(lastHop.body());
	file.close();

	theApp->saveToSettings(UpdateHelpers::updatePatchSavePathKey(), filepath);

	emit updateDownloaded(filepath);
}

}
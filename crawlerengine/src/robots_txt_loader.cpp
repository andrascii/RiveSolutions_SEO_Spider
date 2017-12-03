#include "robots_txt_loader.h"
#include "status_code.h"
#include "download_request.h"
#include "download_response.h"

namespace CrawlerEngine
{

RobotsTxtLoader::RobotsTxtLoader(QObject* parent)
	: QObject(parent)
	, m_isReady(false)
{
}

void RobotsTxtLoader::setUserAgent(const QByteArray& userAgent)
{
	m_userAgent = userAgent;
}

void RobotsTxtLoader::load(const QUrl& url)
{
	if (m_currentLoadedUrl == url)
	{
		return;
	}

	const QString robotsTxtUrlString = url.scheme() + "://" + url.host() + QStringLiteral("/robots.txt");

	CrawlerRequest requestInfo{ QUrl(robotsTxtUrlString), DownloadRequestType::RequestTypeGet };

	DownloadRequest request(requestInfo);
	m_downloadRequester.reset(request, this, &RobotsTxtLoader::onLoadingDone);
	m_downloadRequester->start();
}

const QByteArray& RobotsTxtLoader::content() const noexcept
{
	return m_content;
}

bool RobotsTxtLoader::isReady() const noexcept
{
	return m_isReady;
}

QObject* RobotsTxtLoader::qobject()
{
	return this;
}

void RobotsTxtLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	m_downloadRequester.reset();

	if (!response.redirectUrl.isEmpty())
	{
		CrawlerRequest requestInfo{ response.redirectUrl, DownloadRequestType::RequestTypeGet };

		DownloadRequest request(requestInfo);
		m_downloadRequester.reset(request, this, &RobotsTxtLoader::onLoadingDone);
		m_downloadRequester->start();

		return;
	}

	const Common::StatusCode statusCode = static_cast<Common::StatusCode>(response.statusCode);

	m_content = response.responseBody;
	m_isReady = true;

	m_currentLoadedUrl = response.url;

	emit ready();
}

}

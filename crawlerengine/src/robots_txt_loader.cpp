#include "robots_txt_loader.h"
#include "status_code.h"
#include "download_request.h"
#include "download_response.h"
#include "url.h"

namespace CrawlerEngine
{

RobotsTxtLoader::RobotsTxtLoader(QObject* parent)
	: QObject(parent)
	, m_isReady(false)
	, m_isValid(false)
{
}

void RobotsTxtLoader::load(const Url& host)
{
	if (m_currentLoadedUrl.compare(host))
	{
		return;
	}

	const QString robotsTxtUrlString = host.scheme() + "://" + host.host() + QStringLiteral("/robots.txt");

	CrawlerRequest requestInfo{ Url(robotsTxtUrlString), DownloadRequestType::RequestTypeGet };

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

bool RobotsTxtLoader::isValid() const noexcept
{
	return m_isValid;
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

	m_isValid = statusCode == Common::StatusCode::Ok200;

	m_content = response.responseBody;
	m_isReady = true;

	m_currentLoadedUrl = response.url.host();

	emit ready();
}

}

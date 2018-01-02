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

void RobotsTxtLoader::setHost(const Url& url)
{
	m_host = url;
}

void RobotsTxtLoader::load()
{
	const Url robotsTxtUrl = m_host.scheme() + "://" + m_host.host() + QStringLiteral("/robots.txt");

	if (m_hopsChain.hasHopTo(robotsTxtUrl))
	{
		return;
	}

	CrawlerRequest requestInfo{ robotsTxtUrl, DownloadRequestType::RequestTypeGet };
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
	const Common::StatusCode statusCode = response.hopsChain.back().statusCode();

	m_isValid = statusCode == Common::StatusCode::Ok200;

	m_content = response.hopsChain.back().body();
	m_isReady = true;

	m_hopsChain = response.hopsChain;

	emit ready();
}

}

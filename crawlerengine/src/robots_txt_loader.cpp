#include "stdafx.h"
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

Url RobotsTxtLoader::host() const noexcept
{
	return m_host;
}

void RobotsTxtLoader::load()
{
	const Url robotsTxtUrl = m_host.scheme() + "://" + m_host.host() + QStringLiteral("/robots.txt");

	if (m_hopsChain.hasHopTo(robotsTxtUrl))
	{
		emit ready();
		return;
	}

	CrawlerRequest requestInfo{ robotsTxtUrl, DownloadRequestType::RequestTypeGet };
	DownloadRequest request(requestInfo, 0);
	m_downloadRequester.reset(request, this, &RobotsTxtLoader::onLoadingDone);
	m_downloadRequester->start();
}

const QByteArray& RobotsTxtLoader::content() const noexcept
{
	return m_content;
}

void RobotsTxtLoader::setContent(const QByteArray& content) noexcept
{
	m_content = content;
}

bool RobotsTxtLoader::isReady() const noexcept
{
	return m_isReady;
}

bool RobotsTxtLoader::isValid() const noexcept
{
	return m_isValid;
}

void RobotsTxtLoader::setValid(bool valid) noexcept
{
	m_isReady = true;
	m_isValid = valid;
}

QObject* RobotsTxtLoader::qobject()
{
	return this;
}

CrawlerEngine::Url RobotsTxtLoader::robotsTxtUrl() const
{
	return !m_hopsChain.empty() ? m_hopsChain.lastHop().url() : Url();
}

void RobotsTxtLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	Q_UNUSED(requester);

	const Common::StatusCode statusCode = response.hopsChain.lastHop().statusCode();

	m_isValid = statusCode == Common::StatusCode::Ok200;

	m_content = response.hopsChain.lastHop().body();
	m_isReady = true;

	m_hopsChain = response.hopsChain;

	emit ready();
}

}

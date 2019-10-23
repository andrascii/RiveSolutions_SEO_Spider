#include "stdafx.h"
#include "xml_sitemap_loader.h"
#include "download_response.h"
#include "download_request.h"
#include "robots_txt_rules.h"
#include "url.h"
#include "crawler_request.h"
#include "robots_txt_loader.h"

namespace CrawlerEngine
{

XmlSitemapLoader::XmlSitemapLoader(RobotsTxtLoader* robotsTxtLoader, QObject* parent)
	: QObject(parent)
	, m_isReady(false)
	, m_isValid(false)
	, m_robotsTxtLoader(robotsTxtLoader)
{
	if (m_robotsTxtLoader)
	{
		VERIFY(connect(m_robotsTxtLoader, SIGNAL(ready()), this, SLOT(onRobotsTxtLoaderReady())));
	}
}

void XmlSitemapLoader::setHost(const Url& url)
{
	m_host = url;
}

Url XmlSitemapLoader::host() const noexcept
{
	return m_host;
}

void XmlSitemapLoader::load()
{
	if (m_robotsTxtLoader && !m_robotsTxtLoader->isReady())
	{
		return;
	}

	Url sitemapUrl;

	if (m_robotsTxtLoader && m_robotsTxtLoader->isValid())
	{
		RobotsTxtRules robotsTxtRules(m_robotsTxtLoader->content());

		if (robotsTxtRules.sitemap().isValid())
		{
			sitemapUrl = robotsTxtRules.sitemap();
		}
	}

	if (!sitemapUrl.isValid())
	{
		sitemapUrl = m_host.scheme() + "://" + m_host.host() + QStringLiteral("/sitemap.xml");
	}

	if (m_isValid || m_hopsChain.hasHopTo(sitemapUrl))
	{
		emit ready();
		return;
	}

	CrawlerRequest requestInfo{ sitemapUrl, DownloadRequestType::RequestTypeGet };
	DownloadRequest request{ requestInfo, 0 };

	m_downloadRequester.reset(request, this, &XmlSitemapLoader::onLoadingDone);
	m_downloadRequester->start();
}

const QByteArray& XmlSitemapLoader::content() const noexcept
{
	return m_content;
}

void XmlSitemapLoader::setContent(const QByteArray& content) noexcept
{
	m_content = content;
}

bool XmlSitemapLoader::isReady() const noexcept
{
	return m_isReady;
}

bool XmlSitemapLoader::isValid() const noexcept
{
	return m_isValid;
}

void XmlSitemapLoader::setValid(bool valid) noexcept
{
	m_isReady = true;
	m_isValid = valid;
}

QObject* XmlSitemapLoader::qobject()
{
	return this;
}

void XmlSitemapLoader::onRobotsTxtLoaderReady()
{
	load();
}

void XmlSitemapLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
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
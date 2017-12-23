#include "xml_sitemap_loader.h"
#include "download_response.h"
#include "download_request.h"
#include "robots_txt_rules.h"
#include "custom_url.h"
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
		VERIFY(connect(m_robotsTxtLoader, &RobotsTxtLoader::ready, this, &XmlSitemapLoader::onRobotsTxtLoaderReady));
	}
}

void XmlSitemapLoader::load(const CustomUrl& host)
{
	if (m_isValid && m_currentLoadedUrl.compare(host))
	{
		return;
	}

	m_currentLoadedUrl = host;

	if (m_robotsTxtLoader && !m_robotsTxtLoader->isReady())
	{
		return;
	}

	CustomUrl sitemapUrl;

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
		sitemapUrl = host.scheme() + "://" + host.host() + QStringLiteral("/sitemap.xml");
	}

	CrawlerRequest requestInfo{ sitemapUrl, DownloadRequestType::RequestTypeGet };
	DownloadRequest request{ requestInfo };

	m_downloadRequester.reset(request, this, &XmlSitemapLoader::onLoadingDone);
	m_downloadRequester->start();
}

const QByteArray& XmlSitemapLoader::content() const noexcept
{
	return m_content;
}

bool XmlSitemapLoader::isReady() const noexcept
{
	return m_isReady;
}

bool XmlSitemapLoader::isValid() const noexcept
{
	return m_isValid;
}

QObject* XmlSitemapLoader::qobject()
{
	return this;
}

void XmlSitemapLoader::onRobotsTxtLoaderReady()
{
	load(m_currentLoadedUrl);
}

void XmlSitemapLoader::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	m_downloadRequester.reset();

	if (!response.redirectUrl.isEmpty())
	{
		CrawlerRequest requestInfo{ response.redirectUrl, DownloadRequestType::RequestTypeGet };

		DownloadRequest request(requestInfo);
		m_downloadRequester.reset(request, this, &XmlSitemapLoader::onLoadingDone);
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
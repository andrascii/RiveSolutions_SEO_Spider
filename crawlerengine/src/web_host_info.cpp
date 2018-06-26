#include "web_host_info.h"
#include "proper_404_checker.h"
#include "ispecific_loader.h"
#include "take_screenshot_response.h"
#include "take_screenshot_request.h"

namespace CrawlerEngine
{

WebHostInfo::WebHostInfo(QObject* parent, ISpecificLoader* xmlSiteMapLoader, ISpecificLoader* robotsTxtLoader)
	: QObject(parent)
	, m_xmlSiteMapLoader(xmlSiteMapLoader)
	, m_robotsTxtLoader(robotsTxtLoader)
{
}

void WebHostInfo::reset(const QUrl& url)
{
	m_is404PagesSetupRight = std::nullopt;

	if (m_screenshot.first != url)
	{
		m_screenshot.first = url;
		TakeScreenshotRequest makeScreenshotRequest(url);
		m_screenshotMakerRequester.reset(makeScreenshotRequest, this, &WebHostInfo::onScreenshotCreated);
		m_screenshotMakerRequester->start();
	}

	Check404IsProperRequest request(url);
	m_404IsProperRequester.reset(request, this, &WebHostInfo::on404Checked);
	m_404IsProperRequester->start();
}

std::optional<bool> WebHostInfo::isRobotstxtValid() const
{
	if (!m_robotsTxtLoader->isReady())
	{
		return std::nullopt;
	}

	return m_robotsTxtLoader->isValid();
}

QByteArray WebHostInfo::robotstxtContent() const
{
	return m_robotsTxtLoader->content();
}

Url WebHostInfo::robotstxtUrl() const
{
	return m_robotsTxtLoader->host();
}

std::optional<bool> WebHostInfo::isSiteMapValid() const
{
	if (!m_xmlSiteMapLoader->isReady())
	{
		return std::nullopt;
	}

	return m_xmlSiteMapLoader->isValid();
}

QByteArray WebHostInfo::siteMapContent() const
{
	return m_xmlSiteMapLoader->content();
}

Url WebHostInfo::siteMapUrl() const
{
	return m_xmlSiteMapLoader->host();
}

std::optional<bool> WebHostInfo::is404PagesSetupRight() const
{
	return m_is404PagesSetupRight;
}

const QPixmap& WebHostInfo::screenshot() const
{
	return m_screenshot.second;
}

WebHostInfo::AllData WebHostInfo::allData() const
{
	QByteArray pixmapData;
	if (!screenshot().isNull())
	{
		QBuffer buffer(&pixmapData);

		const QPixmap& pixmap = qvariant_cast<QPixmap>(screenshot());
		pixmap.save(&buffer, "PNG");
	}

	return 
	{ 
		isRobotstxtValid(),
		robotstxtContent(),
		robotstxtUrl(),
		isSiteMapValid(),
		siteMapContent(),
		siteMapUrl(),
		is404PagesSetupRight(), 
		pixmapData
	};
}

void WebHostInfo::setData(const AllData& data)
{
	m_robotsTxtLoader->setValid(data.isRobotstxtValid.value());
	m_robotsTxtLoader->setContent(data.robotstxtContent);
	m_robotsTxtLoader->setHost(data.robotstxtUrl);
	m_xmlSiteMapLoader->setValid(data.isSiteMapValid.value());
	m_xmlSiteMapLoader->setContent(data.siteMapContent);
	m_xmlSiteMapLoader->setHost(data.siteMapUrl);
	m_is404PagesSetupRight = data.is404PagesSetupRight;

	if (!data.image.isEmpty())
	{
		QPixmap hostImage;
		hostImage.loadFromData(data.image, "PNG");

		// TODO: add here the url of the website screenshot
		m_screenshot.second = hostImage;
	}
	else
	{
		// TODO: clear image without constructing QPixmap instance in non-main thread
	}
}

void WebHostInfo::on404Checked(Requester*, const Check404IsProperResponse& response)
{
	m_is404PagesSetupRight = response.result;
}

void WebHostInfo::onScreenshotCreated(Requester*, const ITakeScreenshotResponse& response)
{
	m_screenshotMakerRequester.reset();

	m_screenshot.second = qvariant_cast<QPixmap>(response.screenshot());

	emit webScreenshotLoaded();
}

}

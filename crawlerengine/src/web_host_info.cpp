#include "web_host_info.h"
#include "web_screenshot.h"
#include "proper_404_checker.h"
#include "ispecific_loader.h"

namespace CrawlerEngine
{

WebHostInfo::WebHostInfo(QObject* parent, IWebScreenShot* webScreenShot, 
	ISpecificLoader* xmlSiteMapLoader, ISpecificLoader* robotsTxtLoader)
	: QObject(parent)
	, m_webScreenShot(webScreenShot)
	, m_xmlSiteMapLoader(xmlSiteMapLoader)
	, m_robotsTxtLoader(robotsTxtLoader)
{
	VERIFY(connect(m_webScreenShot->qobject(), SIGNAL(loaded(const QUrl&, const QPixmap&)), this, SIGNAL(webScreenshotLoaded())));
}

void WebHostInfo::reset(const QUrl& url)
{
	m_webScreenShot->load(url);
	m_is404PagesSetupRight = std::nullopt;

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

const QPixmap& WebHostInfo::image() const
{
	return m_webScreenShot->result();
}

WebHostInfo::AllData WebHostInfo::allData() const
{
	QByteArray pixmapData;
	if (!image().isNull())
	{
		QBuffer buffer(&pixmapData);

		const QPixmap& pixmap = qvariant_cast<QPixmap>(image());
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
		m_webScreenShot->setResult(hostImage);
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

}

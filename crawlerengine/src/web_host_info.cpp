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

std::optional<bool> WebHostInfo::isSiteMapValid() const
{
	if (!m_xmlSiteMapLoader->isReady())
	{
		return std::nullopt;
	}

	return m_xmlSiteMapLoader->isValid();
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
	return 
	{ 
		isRobotstxtValid(),
		isSiteMapValid(),
		is404PagesSetupRight(), 
		image() 
	};
}

void WebHostInfo::setData(const AllData& data)
{
	m_robotsTxtLoader->setValid(data.isRobotstxtValid.value());
	m_xmlSiteMapLoader->setValid(data.isSiteMapValid.value());
	m_is404PagesSetupRight = data.is404PagesSetupRight;
	m_webScreenShot->setResult(data.image);
}

void WebHostInfo::on404Checked(Requester*, const Check404IsProperResponse& response)
{
	m_is404PagesSetupRight = response.result;
}

}

#include "web_host_info.h"
#include "web_screenshot.h"
#include "proper_404_checker.h"

namespace CrawlerEngine
{
WebHostInfo::WebHostInfo(QObject* parent, IWebScreenShot* webScreenShot)
	: QObject(parent)
	, m_webScreenShot(webScreenShot)
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

std::optional<bool> WebHostInfo::is404PagesSetupRight() const
{
	return m_is404PagesSetupRight;
}

const QPixmap& WebHostInfo::image() const
{
	return m_webScreenShot->result();
}

void WebHostInfo::on404Checked(Requester*, const Check404IsProperResponse& response)
{
	m_is404PagesSetupRight = response.result;
}

}

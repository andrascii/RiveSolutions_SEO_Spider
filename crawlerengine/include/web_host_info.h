#pragma once

#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{

#ifdef ENABLE_SCREENSHOTS
class IWebScreenShot;
class ITakeScreenshotResponse;
#endif
class ISpecificLoader;

struct Check404IsProperResponse;

class WebHostInfo : public QObject
{
	Q_OBJECT

public:
	WebHostInfo(QObject* parent, ISpecificLoader* xmlSiteMapLoader, ISpecificLoader* robotsTxtLoader);

	void reset(const QUrl& url);

	std::optional<bool> isRobotstxtValid() const;
	QByteArray robotstxtContent() const;
	Url robotstxtUrl() const;
	std::optional<bool> isSiteMapValid() const;
	QByteArray siteMapContent() const;
	Url siteMapUrl() const;
	std::optional<bool> is404PagesSetupRight() const;
#ifdef ENABLE_SCREENSHOTS
	const QPixmap& screenshot() const;
#endif
	struct AllData
	{
		std::optional<bool> isRobotstxtValid;
		QByteArray robotstxtContent;
		Url robotstxtUrl;
		std::optional<bool> isSiteMapValid;
		QByteArray siteMapContent;
		Url siteMapUrl;
		std::optional<bool> is404PagesSetupRight;
		QByteArray image;
	};

	AllData allData() const;
	void setData(const AllData& data);

signals:
#ifdef ENABLE_SCREENSHOTS
	void webScreenshotLoaded();
#endif

private:
	void on404Checked(Requester*, const Check404IsProperResponse& response);
#ifdef ENABLE_SCREENSHOTS
	void onScreenshotCreated(Requester*, const ITakeScreenshotResponse& response);
#endif
	// ... add more

private:
	std::optional<bool> m_is404PagesSetupRight;
#ifdef ENABLE_SCREENSHOTS
	std::pair<Url, QPixmap> m_screenshot;
#endif
	ISpecificLoader* m_xmlSiteMapLoader;
	ISpecificLoader* m_robotsTxtLoader;

	RequesterWrapper m_404IsProperRequester;
#ifdef ENABLE_SCREENSHOTS
	RequesterWrapper m_screenshotMakerRequester;
#endif
};

}

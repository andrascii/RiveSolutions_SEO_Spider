#pragma once
#include "stdafx.h"

#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{

class IWebScreenShot;
class ISpecificLoader;
class ITakeScreenshotResponse;
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
	const QPixmap& screenshot() const;

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
	void webScreenshotLoaded();

private:
	void on404Checked(Requester*, const Check404IsProperResponse& response);
	void onScreenshotCreated(Requester*, const ITakeScreenshotResponse& response);
	// ... add more

private:
	std::optional<bool> m_is404PagesSetupRight;
	std::pair<Url, QPixmap> m_screenshot;
	ISpecificLoader* m_xmlSiteMapLoader;
	ISpecificLoader* m_robotsTxtLoader;

	RequesterWrapper m_404IsProperRequester;
	RequesterWrapper m_screenshotMakerRequester;
};

}

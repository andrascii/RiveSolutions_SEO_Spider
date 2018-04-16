#pragma once
#include "requester_wrapper.h"
#include "proper_404_checker.h"

namespace CrawlerEngine
{
class IWebScreenShot;
class ISpecificLoader;
	
class WebHostInfo : public QObject
{
	Q_OBJECT

public:
	WebHostInfo(QObject* parent, IWebScreenShot* webScreenShot, 
		ISpecificLoader* xmlSiteMapLoader, ISpecificLoader* robotsTxtLoader);
	
	void reset(const QUrl& url);

	std::optional<bool> isRobotstxtValid() const;
	std::optional<bool> isSiteMapValid() const;
	std::optional<bool> is404PagesSetupRight() const;
	const QPixmap& image() const;

	struct AllData
	{
		std::optional<bool> isRobotstxtValid;
		std::optional<bool> isSiteMapValid;
		std::optional<bool> is404PagesSetupRight;
		QPixmap image;
	};

	AllData allData() const;
	void setData(const AllData& data);

private:
	void on404Checked(Requester*, const Check404IsProperResponse& response);
	// ... add more

private:
	std::optional<bool> m_is404PagesSetupRight;
	IWebScreenShot* m_webScreenShot;
	ISpecificLoader* m_xmlSiteMapLoader;
	ISpecificLoader* m_robotsTxtLoader;

	RequesterWrapper m_404IsProperRequester;
};

}

#pragma once
#include "requester_wrapper.h"
#include "proper_404_checker.h"

namespace CrawlerEngine
{
class IWebScreenShot;
	
class WebHostInfo : public QObject
{
	Q_OBJECT

public:
	WebHostInfo(QObject* parent, IWebScreenShot* webScreenShot);
	void reset(const QUrl& url);

	std::optional<bool> is404PagesSetupRight() const;
	const QPixmap& image() const;


private:
	void on404Checked(Requester*, const Check404IsProperResponse& response);
	// ... add more

private:
	std::optional<bool> m_is404PagesSetupRight;
	IWebScreenShot* m_webScreenShot;

	RequesterWrapper m_404IsProperRequester;
};

}

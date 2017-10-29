#pragma once

#include "robots_txt_rules.h"
#include "crawler_options.h"

namespace WebCrawler
{

struct LinkInfo;

class OptionsLinkFilter
{
public:
	enum Permission
	{
		PermissionAllowed,
		PermissionNofollowNotAllowed,
		PermissionBlockedByRobotsTxtRules,
		PermissionSubdomainNotAllowed
	};

	OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules);

	Permission linkPermission(const LinkInfo& linkInfo) const;

private:
	bool isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo) const;

private:
	CrawlerOptions m_crawlerOptions;
	RobotsTxtRules m_robotsTxtRules;
};

}

#pragma once

#include "robots_txt_rules.h"
#include "crawler_options.h"
#include "parsed_page.h"

namespace CrawlerEngine
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
		PermissionSubdomainNotAllowed,
		PermissionBlockedByFolder
	};

	OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules);

	Permission linkPermission(const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const;

private:
	bool isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const;

private:
	CrawlerOptions m_crawlerOptions;
	RobotsTxtRules m_robotsTxtRules;
};

}

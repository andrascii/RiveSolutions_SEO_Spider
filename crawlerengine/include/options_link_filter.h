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
	OptionsLinkFilter(const CrawlerOptionsData& crawlerOptionsData, const RobotsTxtRules& robotsTxtRules);

	bool checkRestriction(Restriction restriction, const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const;
	std::pair<bool, MetaRobotsFlags> isPageBlockedByMetaRobots(const ParsedPagePtr& parsedPage) const;

private:
	bool isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo) const;
	bool isLinkBlockedByMetaRobots(const MetaRobotsFlagsSet& metaRobotsFlags) const;

private:
	CrawlerOptionsData m_crawlerOptionsData;
	RobotsTxtRules m_robotsTxtRules;
};

}

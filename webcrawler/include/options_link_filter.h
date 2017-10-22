#pragma once

#include "robots_txt_rules.h"
#include "crawler_options.h"

namespace WebCrawler
{

struct LinkInfo;

class OptionsLinkFilter
{
public:
	OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules);

	bool isLinkAllowed(const LinkInfo& linkInfo) const;

private:
	CrawlerOptions m_crawlerOptions;
	RobotsTxtRules m_robotsTxtRules;
};

}

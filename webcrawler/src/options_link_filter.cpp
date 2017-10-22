#include "options_link_filter.h"

namespace WebCrawler
{

OptionsLinkFilter::OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules)
	: m_crawlerOptions(crawlerOptions)
	, m_robotsTxtRules(robotsTxtRules)
{
}

bool OptionsLinkFilter::isLinkAllowed(const LinkInfo& linkInfo) const
{
	return true;
}

}

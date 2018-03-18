#include "options_link_filter.h"
#include "parsed_page.h"
#include  "page_parser_helpers.h"

namespace CrawlerEngine
{

OptionsLinkFilter::OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules)
	: m_crawlerOptions(crawlerOptions)
	, m_robotsTxtRules(robotsTxtRules)
{
}

bool OptionsLinkFilter::checkRestriction(Restriction restriction, const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	if(!PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url))
	{
		return false;
	}

	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptions.startCrawlingPage, linkInfo.url);
	const bool isNofollowLink = linkInfo.linkParameter == LinkParameter::NofollowParameter;

	const bool isExternalNofollowNotAllowed = isNofollowLink && isUrlExternal && !m_crawlerOptions.followExternalNofollow;
	const bool isInternalNofollowNotAllowed = isNofollowLink && !isUrlExternal && !m_crawlerOptions.followInternalNofollow;

	if (restriction == Restriction::RestrictionNofollowNotAllowed)
	{
		return isInternalNofollowNotAllowed || isExternalNofollowNotAllowed;
	}

	const bool isSubdomain = PageParserHelpers::isSubdomain(m_crawlerOptions.startCrawlingPage, linkInfo.url);

	if (restriction == Restriction::RestrictionSubdomainNotAllowed)
	{
		return isSubdomain && !m_crawlerOptions.checkSubdomains;
	}

	if (restriction == Restriction::RestrictionBlockedByFolder)
	{
		return !m_crawlerOptions.crawlOutsideOfStartFolder &&
			!PageParserHelpers::isUrlInsideBaseUrlFolder(m_crawlerOptions.startCrawlingPage, linkInfo.url);
	}

	if (restriction == Restriction::RestrictionExternalLinksNotAllowed)
	{
		return !isNofollowLink && !isSubdomain && isUrlExternal && !m_crawlerOptions.checkExternalLinks;
	}

	if (restriction == Restriction::RestrictionBlockedByRobotsTxtRules)
	{
		return !isUrlExternal && m_crawlerOptions.followRobotsTxtRules && 
			isLinkBlockedByRobotsTxt(linkInfo);
	}
	
	if (restriction == Restriction::RestrictionBlockedByMetaRobotsRules)
	{
		return !isUrlExternal && m_crawlerOptions.followRobotsTxtRules && 
			isLinkBlockedByMetaRobots(metaRobotsFlags);
	}

	return false;
}

std::pair<bool, MetaRobotsFlags> OptionsLinkFilter::isPageBlockedByMetaRobots(const ParsedPagePtr& parsedPage) const
{
	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptions.startCrawlingPage, parsedPage->url);

	const std::pair<bool, UserAgentType> isAllowedForRobot = m_robotsTxtRules.isUrlAllowedByMetaRobotsFor(parsedPage->metaRobotsFlags, m_crawlerOptions.userAgentToFollow);

	return std::make_pair(!isUrlExternal && m_crawlerOptions.followRobotsTxtRules && !isAllowedForRobot.first, parsedPage->metaRobotsFlags[isAllowedForRobot.second]);
}

bool OptionsLinkFilter::isLinkBlockedByMetaRobots(const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	return !m_robotsTxtRules.isUrlAllowedByMetaRobots(metaRobotsFlags, m_crawlerOptions.userAgentToFollow);
}

bool OptionsLinkFilter::isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo) const
{
	return !m_robotsTxtRules.isUrlAllowedByRobotsTxt(linkInfo.url, m_crawlerOptions.userAgentToFollow);
}

}

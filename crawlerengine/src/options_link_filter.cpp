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

bool OptionsLinkFilter::checkPermissionNotAllowed(Permission permission, const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	DEBUG_ASSERT(PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url));

	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptions.startCrawlingPage, linkInfo.url);
	const bool isNofollowLink = linkInfo.urlParameter == LinkParameter::NofollowParameter;

	const bool isExternalNofollowNotAllowed = isNofollowLink && isUrlExternal && !m_crawlerOptions.followExternalNofollow;
	const bool isInternalNofollowNotAllowed = isNofollowLink && !isUrlExternal && !m_crawlerOptions.followInternalNofollow;

	if (permission == PermissionNofollowNotAllowed)
	{
		return isInternalNofollowNotAllowed || isExternalNofollowNotAllowed;
	}

	const bool isSubdomain = PageParserHelpers::isSubdomain(m_crawlerOptions.startCrawlingPage, linkInfo.url);

	if (permission == PermissionSubdomainNotAllowed)
	{
		return isSubdomain && !m_crawlerOptions.checkSubdomains;
	}

	if (permission == PermissionBlockedByFolder)
	{
		return !m_crawlerOptions.crawlOutsideOfStartFolder &&
			!PageParserHelpers::isUrlInsideBaseUrlFolder(m_crawlerOptions.startCrawlingPage, linkInfo.url);
	}

	if (permission == PermissionExternalLinksNotAllowed)
	{
		return !isNofollowLink && !isSubdomain && isUrlExternal && !m_crawlerOptions.checkExternalLinks;
	}

	if (permission == PermissionBlockedByRobotsTxtRules)
	{
		return !isUrlExternal && m_crawlerOptions.followRobotsTxtRules && 
			isLinkBlockedByRobotsTxt(linkInfo);
	}
	
	if (permission == PermissionBlockedByMetaRobotsRules)
	{
		return !isUrlExternal && m_crawlerOptions.followRobotsTxtRules && 
			isLinkBlockedByMetaRobots(metaRobotsFlags);
	}

	return false;
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

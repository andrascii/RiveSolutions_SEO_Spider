#include "options_link_filter.h"
#include "parsed_page.h"
#include  "page_parser_helpers.h"

namespace WebCrawler
{

OptionsLinkFilter::OptionsLinkFilter(const CrawlerOptions& crawlerOptions, const RobotsTxtRules& robotsTxtRules)
	: m_crawlerOptions(crawlerOptions)
	, m_robotsTxtRules(robotsTxtRules)
{
}

OptionsLinkFilter::Permission OptionsLinkFilter::linkPermission(const LinkInfo& linkInfo) const
{
	DEBUG_ASSERT(PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url));

	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptions.host, linkInfo.url);
	const bool isNofollowLink = linkInfo.urlParameter == LinkParameter::NofollowParameter;

	const bool isExternalNofollowNotAllowed = isNofollowLink && isUrlExternal && !m_crawlerOptions.followExternalNofollow;
	const bool isInternalNofollowNotAllowed = isNofollowLink && !isUrlExternal && !m_crawlerOptions.followInternalNofollow;

	if (isExternalNofollowNotAllowed || isInternalNofollowNotAllowed)
	{
		return PermissionNofollowNotAllowed;
	}
	
	if (!isUrlExternal && m_crawlerOptions.followRobotsTxtRules && isLinkBlockedByRobotsTxt(linkInfo))
	{
		return PermissionBlockedByRobotsTxtRules;
	}

	const bool isSubdomain = PageParserHelpers::isSubdomain(m_crawlerOptions.host, linkInfo.url);

	if (isSubdomain && !m_crawlerOptions.checkSubdomains)
	{
		return PermissionSubdomainNotAllowed;
	}

	return PermissionAllowed;
}

bool OptionsLinkFilter::isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo) const
{
	return !m_robotsTxtRules.isUrlAllowed(linkInfo.url, m_crawlerOptions.userAgentToFollow);
}

}

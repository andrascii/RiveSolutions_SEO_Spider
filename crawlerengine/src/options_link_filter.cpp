#include "stdafx.h"
#include "options_link_filter.h"
#include "parsed_page.h"
#include  "page_parser_helpers.h"

namespace CrawlerEngine
{

OptionsLinkFilter::OptionsLinkFilter(const CrawlerOptionsData& crawlerOptionsData, const RobotsTxtRules& robotsTxtRules)
	: m_crawlerOptionsData(crawlerOptionsData)
	, m_robotsTxtRules(robotsTxtRules)
{
}

const Url& OptionsLinkFilter::startCrawlingPage() const noexcept
{
	return m_crawlerOptionsData.startCrawlingPage;
}

bool OptionsLinkFilter::checkRestriction(Restriction restriction, const LinkInfo& linkInfo, const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	if(!PageParserHelpers::isHttpOrHttpsScheme(linkInfo.url))
	{
		return false;
	}

	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptionsData.startCrawlingPage, linkInfo.url, m_crawlerOptionsData.checkSubdomains);
	const bool isNofollowLink = linkInfo.linkParameter == LinkParameter::NofollowParameter;

	const bool isExternalNofollowNotAllowed = isNofollowLink && isUrlExternal && !m_crawlerOptionsData.followExternalNofollow;
	const bool isInternalNofollowNotAllowed = isNofollowLink && !isUrlExternal && !m_crawlerOptionsData.followInternalNofollow;

	if (restriction == Restriction::RestrictionNofollowNotAllowed)
	{
		return isInternalNofollowNotAllowed || isExternalNofollowNotAllowed;
	}

	const bool isSubdomain = PageParserHelpers::isSubdomain(m_crawlerOptionsData.startCrawlingPage, linkInfo.url);

	if (restriction == Restriction::RestrictionSubdomainNotAllowed)
	{
		return isSubdomain && !m_crawlerOptionsData.checkSubdomains;
	}

	if (restriction == Restriction::RestrictionBlockedByFolder)
	{
		return !isSubdomain && !m_crawlerOptionsData.crawlOutsideOfStartFolder &&
			!PageParserHelpers::isUrlInsideBaseUrlFolder(m_crawlerOptionsData.startCrawlingPage, linkInfo.url);
	}

	if (restriction == Restriction::RestrictionExternalLinksNotAllowed)
	{
		return isUrlExternal && !m_crawlerOptionsData.checkExternalLinks;
	}

	if (restriction == Restriction::RestrictionBlockedByRobotsTxtRules)
	{
		return !isUrlExternal && m_crawlerOptionsData.followRobotsTxtRules && 
			isLinkBlockedByRobotsTxt(linkInfo);
	}
	
	if (restriction == Restriction::RestrictionBlockedByMetaRobotsRules)
	{
		return !isUrlExternal && m_crawlerOptionsData.followRobotsTxtRules && 
			isLinkBlockedByMetaRobots(metaRobotsFlags);
	}

	return false;
}

std::pair<bool, MetaRobotsFlags> OptionsLinkFilter::isPageBlockedByMetaRobots(const ParsedPagePtr& parsedPage) const
{
	const bool isUrlExternal = PageParserHelpers::isUrlExternal(m_crawlerOptionsData.startCrawlingPage, parsedPage->url, m_crawlerOptionsData.checkSubdomains);

	const std::pair<bool, UserAgentType> isAllowedForRobot = m_robotsTxtRules.isUrlAllowedByMetaRobotsFor(parsedPage->metaRobotsFlags, m_crawlerOptionsData.userAgentToFollow);

	return std::make_pair(!isUrlExternal && m_crawlerOptionsData.followRobotsTxtRules && !isAllowedForRobot.first, parsedPage->metaRobotsFlags[isAllowedForRobot.second]);
}

bool OptionsLinkFilter::isLinkBlockedByMetaRobots(const MetaRobotsFlagsSet& metaRobotsFlags) const
{
	return !m_robotsTxtRules.isUrlAllowedByMetaRobots(metaRobotsFlags, m_crawlerOptionsData.userAgentToFollow);
}

bool OptionsLinkFilter::isLinkBlockedByRobotsTxt(const LinkInfo& linkInfo) const
{
	return !m_robotsTxtRules.isUrlAllowedByRobotsTxt(linkInfo.url, m_crawlerOptionsData.userAgentToFollow);
}

}

#include "robots_txt_yahoo_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

bool RobotsTxtYahooStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return true;
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;

	return !flags.testFlag(MetaRobotsNoFollow);
}

}
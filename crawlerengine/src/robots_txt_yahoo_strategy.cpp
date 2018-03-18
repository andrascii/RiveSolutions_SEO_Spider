#include "robots_txt_yahoo_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

	std::pair<bool, UserAgentType> RobotsTxtYahooStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return std::make_pair(true, userAgentType);
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;

	return std::make_pair(!flags.testFlag(MetaRobotsNoFollow), userAgentType);
}

}

#include "robots_txt_google_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

std::pair<bool, UserAgentType> RobotsTxtGoogleStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return std::make_pair(true, userAgentType);
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;

	// all and follow are not supported
	return std::make_pair(!flags.testFlag(MetaRobotsNone) && !flags.testFlag(MetaRobotsNoFollow) && !flags.testFlag(MetaRobotsNoIndex), userAgentType);
}

}

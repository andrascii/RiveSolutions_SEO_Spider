#include "robots_txt_google_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

bool RobotsTxtGoogleStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return true;
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;

	// all and follow are not supported
	return !flags.testFlag(MetaRobotsNone) &&
		!flags.testFlag(MetaRobotsNoFollow);
}

}

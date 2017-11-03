#include "robots_txt_mail_ru_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

bool RobotsTxtMailRuStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return true;
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;
	
	return flags.testFlag(MetaRobotsAll) ||
		flags.testFlag(MetaRobotsFollow) ||
		(!flags.testFlag(MetaRobotsNone) &&
		!flags.testFlag(MetaRobotsNoFollow));
}

}

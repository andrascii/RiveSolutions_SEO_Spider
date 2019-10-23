#include "stdafx.h"
#include "robots_txt_mail_ru_strategy.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

std::pair<bool, UserAgentType> RobotsTxtMailRuStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return std::make_pair(true, userAgentType);
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;
	
	return std::make_pair(flags.testFlag(MetaRobotsAll) || flags.testFlag(MetaRobotsFollow) ||
		(!flags.testFlag(MetaRobotsNone) && !flags.testFlag(MetaRobotsNoFollow)), userAgentType);
}

}

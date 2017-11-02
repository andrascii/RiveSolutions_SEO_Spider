#include "robots_txt_google_strategy.h"

namespace CrawlerEngine
{

bool RobotsTxtGoogleStrategy::isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const
{
	// all and follow are not supported
	return !metaRobotsFlags.testFlag(MetaRobotsNone) &&
		!metaRobotsFlags.testFlag(MetaRobotsNoFollow);
}

}

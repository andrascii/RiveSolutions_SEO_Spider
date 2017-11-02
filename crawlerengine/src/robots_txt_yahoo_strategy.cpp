#include "robots_txt_yahoo_strategy.h"

namespace CrawlerEngine
{

bool RobotsTxtYahooStrategy::isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const
{
	return !metaRobotsFlags.testFlag(MetaRobotsNoFollow);
}

}

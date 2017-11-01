#include "robots_txt_yahoo_strategy.h"

namespace WebCrawler
{

bool RobotsTxtYahooStrategy::isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const
{
	return !metaRobotsFlags.testFlag(MetaRobotsNoFollow);
}

}

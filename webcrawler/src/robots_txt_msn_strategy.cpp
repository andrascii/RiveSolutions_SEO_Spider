#include "robots_txt_msn_strategy.h"

namespace WebCrawler
{

bool RobotsTxtMsnStrategy::isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const
{
	return !metaRobotsFlags.testFlag(MetaRobotsNoFollow);
}

}

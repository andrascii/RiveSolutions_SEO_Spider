#include "robots_txt_mail_ru_strategy.h"

namespace CrawlerEngine
{

bool RobotsTxtMailRuStrategy::isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const
{
	return metaRobotsFlags.testFlag(MetaRobotsAll) ||
		metaRobotsFlags.testFlag(MetaRobotsFollow) ||
		(!metaRobotsFlags.testFlag(MetaRobotsNone) &&
		!metaRobotsFlags.testFlag(MetaRobotsNoFollow));
}

}

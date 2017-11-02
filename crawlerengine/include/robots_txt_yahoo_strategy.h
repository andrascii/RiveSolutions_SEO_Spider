#pragma once

#include "robots_txt_base_strategy.h"

namespace CrawlerEngine
{

// Slurp
// https://help.yahoo.com/kb/guide-meta-tags-robotstxt-slurp-sln2213.html
	
class RobotsTxtYahooStrategy : public RobotsTxtBaseStrategy
{
	virtual bool isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const override;
};

}
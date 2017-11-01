#pragma once

#include "robots_txt_base_strategy.h"

namespace WebCrawler
{

// googlebot
// https://developers.google.com/webmasters/control-crawl-index/docs/robots_txt
// https://developers.google.com/webmasters/control-crawl-index/docs/robots_meta_tag
	
class RobotsTxtGoogleStrategy : public RobotsTxtBaseStrategy
{
	virtual bool isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const override;
};

}
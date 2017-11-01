#pragma once

#include "robots_txt_base_strategy.h"

namespace WebCrawler
{

// msnbot
// https://blogs.bing.com/webmaster/2009/08/20/prevent-a-bot-from-getting-lost-in-space-sem-101
// https://blogs.bing.com/webmaster/2008/06/03/robots-exclusion-protocol-joining-together-to-provide-better-documentation

class RobotsTxtMsnStrategy : public RobotsTxtBaseStrategy
{
	virtual bool isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const override;
};

}
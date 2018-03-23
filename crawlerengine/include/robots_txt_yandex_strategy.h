#pragma once

#include "robots_txt_base_strategy.h"

namespace CrawlerEngine
{

// yandex
// https://yandex.ru/support/webmaster/controlling-robot/robots-txt.html	

class RobotsTxtYandexStrategy : public RobotsTxtBaseStrategy
{
public:
	virtual Url cleanUrl(const Url& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const override;
	virtual std::pair<bool, UserAgentType> isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
};


}
#pragma once

#include "robots_txt_base_strategy.h"

namespace CrawlerEngine
{

// yandex
// https://yandex.ru/support/webmaster/controlling-robot/robots-txt.html	

class RobotsTxtYandexStrategy : public RobotsTxtBaseStrategy
{
public:
	virtual QUrl cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const override;
	virtual bool isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
};


}
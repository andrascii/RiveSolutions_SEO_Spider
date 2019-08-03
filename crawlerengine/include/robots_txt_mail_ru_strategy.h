#pragma once
#include "stdafx.h"

#include "robots_txt_yandex_strategy.h"

namespace CrawlerEngine
{

// https://help.mail.ru/webmaster/indexing/robots.txt/rules/user-agent
// https://help.mail.ru/webmaster/indexing/robots/management_methods/meta_tags

class RobotsTxtMailRuStrategy : public RobotsTxtYandexStrategy
{
	virtual std::pair<bool, UserAgentType> isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
};

}
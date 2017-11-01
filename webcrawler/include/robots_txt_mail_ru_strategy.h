#pragma once

#include "robots_txt_yandex_strategy.h"

namespace WebCrawler
{
	
// https://help.mail.ru/webmaster/indexing/robots.txt/rules/user-agent
// https://help.mail.ru/webmaster/indexing/robots/management_methods/meta_tags

class RobotsTxtMailRuStrategy : public RobotsTxtYandexStrategy
{
	virtual bool isUrlAllowed(MetaRobotsFlags metaRobotsFlags) const override;
};

}
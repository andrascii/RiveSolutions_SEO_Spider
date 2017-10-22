#pragma once

#include "robots_txt_base_strategy.h"

namespace WebCrawler
{
	

class RobotsTxtYandexStrategy : public RobotsTxtBaseStrategy
{
public:
	virtual QUrl cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;
};


}
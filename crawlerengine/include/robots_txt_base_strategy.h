#pragma once
#include "parsed_page.h"

namespace CrawlerEngine
{
	enum class UserAgentType;
	class RobotsTxtTokenizer;

class RobotsTxtBaseStrategy
{
public:
	virtual ~RobotsTxtBaseStrategy() = default;

	virtual bool isUrlAllowed(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;
	virtual bool isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const;
	virtual QUrl cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;

protected:
	bool patternMatched(const QString& pattern, const QString& value) const;
};

}
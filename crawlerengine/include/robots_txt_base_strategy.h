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

	virtual bool isUrlAllowed(const Url& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;
	virtual std::pair<bool, UserAgentType> isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const;
	virtual Url cleanUrl(const Url& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const;

protected:
	bool patternMatched(const QString& pattern, const QString& value) const;
};

}

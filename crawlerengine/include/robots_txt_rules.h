#pragma once

#include "parsed_page.h"
#include "icrawler_options.h"

namespace CrawlerEngine
{

class ISpecificLoader;
class RobotsTxtTokenizer;
class RobotsTxtBaseStrategy;

class IRobotsTxtRules
{
public:
    virtual ~IRobotsTxtRules() = default;
	virtual bool isValid() const = 0;
	virtual bool isUrlAllowedByRobotsTxt(const Url& url, UserAgentType userAgentType) const = 0;
	virtual bool isUrlAllowedByMetaRobots(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const = 0;
	virtual std::pair<bool, UserAgentType> isUrlAllowedByMetaRobotsFor(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const = 0;
	virtual const Url& sitemap() const = 0;
	virtual const Url& originalHostMirror() const = 0;
};

class RobotsTxtRules : public IRobotsTxtRules
{
public:
	RobotsTxtRules();
	RobotsTxtRules(const QByteArray& content);

	virtual bool isValid() const override;
	virtual bool isUrlAllowedByRobotsTxt(const Url& url, UserAgentType userAgentType) const override;
	virtual bool isUrlAllowedByMetaRobots(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
	virtual std::pair<bool, UserAgentType> isUrlAllowedByMetaRobotsFor(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
	virtual const Url& sitemap() const override;
	virtual const Url& originalHostMirror() const override;

private:
	std::shared_ptr<RobotsTxtTokenizer> m_tokenizer;
	QMap<UserAgentType, std::shared_ptr<RobotsTxtBaseStrategy>> m_strategies;
};

Q_DECLARE_METATYPE(RobotsTxtRules)

}

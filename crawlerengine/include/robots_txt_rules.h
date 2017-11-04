#pragma once
#include "parsed_page.h"

namespace CrawlerEngine
{

class IRobotsTxtLoader;
class RobotsTxtTokenizer;
class RobotsTxtBaseStrategy;



class IRobotsTxtRules
{
public:
	virtual bool isValid() const = 0;
	virtual bool isUrlAllowed(const QUrl& url, const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const = 0;
	virtual const QUrl& sitemap() const = 0;
	virtual const QUrl& originalHostMirror() const = 0;
};

class RobotsTxtRules : public IRobotsTxtRules
{
public:
	RobotsTxtRules();
	RobotsTxtRules(const QByteArray& content);

	virtual bool isValid() const override;
	virtual bool isUrlAllowed(const QUrl& url, const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const override;
	virtual const QUrl& sitemap() const override;
	virtual const QUrl& originalHostMirror() const override;

private:
	std::shared_ptr<RobotsTxtTokenizer> m_tokenizer;
	QMap<UserAgentType, std::shared_ptr<RobotsTxtBaseStrategy>> m_strategies;
};

Q_DECLARE_METATYPE(RobotsTxtRules)

}

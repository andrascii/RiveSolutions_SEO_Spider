#include "robots_txt_rules.h"
#include "robots_txt_tokenizer.h"
#include "robots_txt_base_strategy.h"
#include "robots_txt_yandex_strategy.h"
#include "page_parser_helpers.h"
#include "robots_txt_mail_ru_strategy.h"
#include "robots_txt_yahoo_strategy.h"
#include "robots_txt_msn_strategy.h"
#include "robots_txt_google_strategy.h"

namespace CrawlerEngine
{

RobotsTxtRules::RobotsTxtRules()
{
	ASSERT(!"This constructor intended only for ensure qRegisterMetatype is working");
}

RobotsTxtRules::RobotsTxtRules(const QByteArray& content)
	: m_tokenizer(new RobotsTxtTokenizer)
{
	m_tokenizer->tokenize(content);

	m_strategies[UserAgentType::AnyBot] = std::make_shared<RobotsTxtBaseStrategy>();
	m_strategies[UserAgentType::YandexBot] = std::make_shared<RobotsTxtYandexStrategy>();
	m_strategies[UserAgentType::MailRuBot] = std::make_shared<RobotsTxtMailRuStrategy>();
	m_strategies[UserAgentType::YahooBot] = std::make_shared<RobotsTxtYahooStrategy>();
	m_strategies[UserAgentType::MsnBot] = std::make_shared<RobotsTxtMsnStrategy>();
	m_strategies[UserAgentType::GoogleBot] = std::make_shared<RobotsTxtGoogleStrategy>();
}

bool RobotsTxtRules::isValid() const
{
	return m_tokenizer->isValid();
}

bool RobotsTxtRules::isUrlAllowed(const QUrl& url, const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	DEBUG_ASSERT(!url.isRelative());
	DEBUG_ASSERT(PageParserHelpers::isHttpOrHttpsScheme(url));

	if (m_strategies.find(userAgentType) == m_strategies.cend())
	{
		WARNLOG << "No appropriate strategy was found";
		userAgentType = UserAgentType::AnyBot;
	}

	return m_strategies[userAgentType]->isUrlAllowed(url, userAgentType, *m_tokenizer) &&
		m_strategies[userAgentType]->isUrlAllowed(metaRobotsFlags, userAgentType);
}

const QUrl& RobotsTxtRules::sitemap() const
{
	return m_tokenizer->sitemap();
}

const QUrl& RobotsTxtRules::originalHostMirror() const
{
	return m_tokenizer->originalHostMirror();
}

}

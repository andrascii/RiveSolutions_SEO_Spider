#include "robots_txt_rules.h"
#include "robots_txt_tokenizer.h"
#include "irobots_txt_loader.h"
#include "robots_txt_base_strategy.h"
#include "robots_txt_yandex_strategy.h"

namespace WebCrawler
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
	m_strategies[UserAgentType::YahooBot] = std::make_shared<RobotsTxtYandexStrategy>();
}

bool RobotsTxtRules::isValid() const
{
	return m_tokenizer->isValid();
}

bool RobotsTxtRules::isUrlAllowed(const QUrl& url, UserAgentType userAgentType) const
{
	if (m_strategies.find(userAgentType) == m_strategies.cend())
	{
		WARNINGLOG << "No appropriate strategy was found";
		m_strategies[UserAgentType::AnyBot]->isUrlAllowed(url, userAgentType, *m_tokenizer);
	}

	return m_strategies[userAgentType]->isUrlAllowed(url, userAgentType, *m_tokenizer);
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

#include "robots_txt_rules.h"
#include "robots_txt_tokenizer.h"
#include "irobots_txt_loader.h"

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
}

bool RobotsTxtRules::isValid() const
{
	return m_tokenizer->isValid();
}

bool RobotsTxtRules::isUrlAllow(const QUrl& url, UserAgentType userAgentType) const
{
	return false;
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
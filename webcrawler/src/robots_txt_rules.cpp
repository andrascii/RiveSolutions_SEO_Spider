#include "robots_txt_rules.h"
#include "robots_txt_tokenizer.h"
#include "irobots_txt_loader.h"

namespace WebCrawler
{

//////////////////////////////////////////////////////////////////////////

RobotsTxtRules::RobotsTxtRules(IRobotsTxtLoader* loader, QObject* parent)
	: QObject(parent)														
	, m_loader(loader)
	, m_valid(false)
	, m_initialized(false)
	, m_tokenizer(new RobotsTxtTokenizer)
{
	VERIFY(connect(loader->qObject(), SIGNAL(ready(const QByteArray&)), this, SLOT(onLoadingDone(const QByteArray&))));
}

bool RobotsTxtRules::isValid() const
{
	return !m_valid ? false : m_tokenizer->isValid();
}

bool RobotsTxtRules::isInitialized() const
{
	return m_initialized;
}

void RobotsTxtRules::initRobotsTxt(const QUrl& url)
{
	m_valid = false;
	m_initialized = false;

	m_loader->load(url);
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

QObject* RobotsTxtRules::qobject() const
{
	return static_cast<QObject*>(const_cast<RobotsTxtRules* const>(this));
}

void RobotsTxtRules::onLoadingDone(const QByteArray& content)
{
	m_tokenizer->tokenize(content);

	m_valid = true;
	m_initialized = true;

	emit initialized();
}

}
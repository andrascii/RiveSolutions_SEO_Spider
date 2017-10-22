#pragma once

namespace WebCrawler
{

class IRobotsTxtLoader;
class RobotsTxtTokenizer;

enum class UserAgentType
{
	InvalidBot,
	GoogleBot,
	YandexBot,
	MailRuBot,
	YahooBot,
	MsnBot,
	AltaVistaBot,
	RamblerBot,
	AportBot,
	WebAltaBot,
	AnyBot // used for all robots
};

class IRobotsTxtRules
{
public:
	virtual bool isValid() const = 0;
	virtual bool isUrlAllowed(const QUrl& url, UserAgentType userAgentType) const = 0;
	virtual const QUrl& sitemap() const = 0;
	virtual const QUrl& originalHostMirror() const = 0;
};

class RobotsTxtRules : public IRobotsTxtRules
{
public:
	RobotsTxtRules();
	RobotsTxtRules(const QByteArray& content);

	virtual bool isValid() const override;
	virtual bool isUrlAllowed(const QUrl& url, UserAgentType userAgentType) const override;
	virtual const QUrl& sitemap() const override;
	virtual const QUrl& originalHostMirror() const override;

private:
	std::shared_ptr<RobotsTxtTokenizer> m_tokenizer;
};

Q_DECLARE_METATYPE(RobotsTxtRules)

}
#pragma once

namespace WebCrawler
{

class RobotsTxtTokenizer;

enum class UserAgentType
{
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
	virtual bool isInitialized() const = 0;
	virtual void initRobotsTxt(const QUrl& url) = 0;
	virtual bool isUrlAllow(const QUrl& url, UserAgentType userAgentType) const = 0;
	virtual const QUrl& sitemap() const = 0;
	virtual const QUrl& originalHostMirror() const = 0;

	virtual QObject* qobject() const = 0;

	// signals
	virtual void initialized() = 0;
};

class RobotsTxtRules : public QObject, public IRobotsTxtRules
{
	Q_OBJECT

public:
	RobotsTxtRules(QNetworkAccessManager* networkAccessor, QObject* parent = nullptr);

	virtual bool isValid() const override;
	virtual bool isInitialized() const override;
	virtual void initRobotsTxt(const QUrl& url) override;
	virtual bool isUrlAllow(const QUrl& url, UserAgentType userAgentType) const override;
	virtual const QUrl& sitemap() const override;
	virtual const QUrl& originalHostMirror() const override;

	virtual QObject* qobject() const override;

signals:
	virtual void initialized() override;

private slots:
	void onLoadingDone(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_networkAccessor;

	bool m_valid;
	bool m_initialized;

	std::unique_ptr<RobotsTxtTokenizer> m_tokenizer;
};

class RobotsTxtRulesTestStub : public RobotsTxtRules
{
public:
	virtual void initRobotsTxt(const QUrl& url) override;
};

}
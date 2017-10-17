#include "robots_txt_rules.h"
#include "status_code.h"

namespace WebCrawler
{

enum class RobotsTxtToken
{
	TokenUserAgent,
	TokenAllow,
	TokenDisallow,
	TokenSitemap,
	TokenHost,
	TokenCrawlDelay,
	TokenCleanParam,
	TokenCommentary,
	TokenStringDelimeter,
	TokenUnknown
};

namespace
{

const QMap<RobotsTxtToken, QString> s_tokenToString =
{
	{ RobotsTxtToken::TokenUserAgent, "user-agent" },
	{ RobotsTxtToken::TokenAllow, "allow" },
	{ RobotsTxtToken::TokenDisallow, "disallow" },
	{ RobotsTxtToken::TokenSitemap, "sitemap" },
	{ RobotsTxtToken::TokenHost, "host" },
	{ RobotsTxtToken::TokenCrawlDelay, "crawl-delay" },
	{ RobotsTxtToken::TokenCleanParam, "clean-param" },
	{ RobotsTxtToken::TokenCommentary, "#" },
	{ RobotsTxtToken::TokenStringDelimeter, ":" }
};

const QMap<QString, RobotsTxtToken> s_stringToToken =
{
	{ "user-agent", RobotsTxtToken::TokenUserAgent },
	{ "allow", RobotsTxtToken::TokenAllow },
	{ "disallow", RobotsTxtToken::TokenDisallow },
	{ "sitemap", RobotsTxtToken::TokenSitemap },
	{ "host", RobotsTxtToken::TokenHost },
	{ "crawl-delay", RobotsTxtToken::TokenCrawlDelay },
	{ "clean-param", RobotsTxtToken::TokenCleanParam },
	{ "#", RobotsTxtToken::TokenCommentary },
	{ ":", RobotsTxtToken::TokenStringDelimeter }
};

const QMap<QString, UserAgentType> s_userAgent =
{
	{ "googlebot", UserAgentType::GoogleBot },
	{ "yandex", UserAgentType::YandexBot },
	{ "mail.ru", UserAgentType::MailRuBot },
	{ "msnbot", UserAgentType::MsnBot },
	{ "*", UserAgentType::AnyBot }
};

class AnywayFunctionCall
{
public:
	AnywayFunctionCall(const std::function<void()>& function)
		: m_function(function)
	{
	}

	~AnywayFunctionCall()
	{
		m_function();
	}

private:
	std::function<void()> m_function;
};

}

class RobotsTxtTokenizer final
{
public:
	RobotsTxtTokenizer()
		: m_validRobotsTxt(false)
	{
	}

	bool isValid() const noexcept
	{
		return m_validRobotsTxt;
	}

	void tokenize(const QString& robotsTxtContent)
	{
		QStringList rows = removeCommentaries(robotsTxtContent.split("\n", QString::SkipEmptyParts));

		UserAgentType userAgentType = UserAgentType::AnyBot;

		for(int i = 0; i < rows.size(); ++i)
		{
			auto [token, tokenValue] = tokenizeRow(rows[i]);

			const bool isUserAgentToken = token == s_tokenToString[RobotsTxtToken::TokenUserAgent];
			const bool isSitemapToken = token == s_tokenToString[RobotsTxtToken::TokenSitemap];
			const bool isHostToken = token == s_tokenToString[RobotsTxtToken::TokenHost];

			if (!isUserAgentToken && !isSitemapToken && !isHostToken && i == 0)
			{
				//
				// First token must be a user-agent or sitemap or host
				//
				m_validRobotsTxt = false;
				return;
			}

			if (isUserAgentToken)
			{
				userAgentType = s_userAgent[tokenValue];
			}
			else
			{
				RobotsTxtToken tokenEnumerator = s_stringToToken.value(token, RobotsTxtToken::TokenUnknown);

				Tokens& tokens = m_userAgentTokens[userAgentType];
				tokens.insert(tokenEnumerator, tokenValue);
			}
		}

		m_validRobotsTxt = true;
	}

	QList<QString> tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const
	{
		return m_userAgentTokens.contains(userAgentType) ? 
			m_userAgentTokens.value(userAgentType).values(token) : QList<QString>();
	}

	const QUrl& sitemap() const
	{
		return m_sitemapUrl;
	}

	const QUrl& originalHostMirror() const
	{
		return m_originalHostMirrorUrl;
	}

private:
	QStringList removeCommentaries(const QStringList& strings)
	{
		QStringList resultStrings;

		foreach(QString string, strings)
		{
			if (string.remove(QRegularExpression("#.*")).isEmpty())
			{
				continue;
			}

			resultStrings << string;
		}

		return resultStrings;
	}

	QPair<QString, QString> tokenizeRow(const QString& row) const
	{
		const int tokenPartStringDelimeterPosition =
			row.indexOf(s_tokenToString[RobotsTxtToken::TokenStringDelimeter]);

		if (tokenPartStringDelimeterPosition == -1)
		{
			// invalid row
			return qMakePair(QString::null, QString::null);
		}

		const QString token = row.left(tokenPartStringDelimeterPosition).trimmed().toLower();
		const QString tokenValue = row.right(row.size() - tokenPartStringDelimeterPosition - 1).trimmed().toLower();

		return qMakePair(token, tokenValue);
	}

private:
	using Tokens = QMultiMap<RobotsTxtToken, QString>;

	QUrl m_sitemapUrl;
	QUrl m_originalHostMirrorUrl;
	QMap<UserAgentType, Tokens> m_userAgentTokens;
	bool m_validRobotsTxt;
};

//////////////////////////////////////////////////////////////////////////

RobotsTxtRules::RobotsTxtRules(QNetworkAccessManager* networkAccessor, QObject* parent)
	: QObject(parent)														
	, m_networkAccessor(networkAccessor)
	, m_valid(false)
	, m_initialized(false)
	, m_tokenizer(new RobotsTxtTokenizer)
{
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
	ASSERT(m_networkAccessor->thread() == QThread::currentThread());

	m_valid = false;
	m_initialized = false;

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*))));

	const QString robotsTxt = url.scheme() + "://" + url.host() + QStringLiteral("/robots.txt");
	QNetworkReply* reply = m_networkAccessor->get(QNetworkRequest(robotsTxt));
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

void RobotsTxtRules::onLoadingDone(QNetworkReply* reply)
{
	AnywayFunctionCall anywayFunctionCall(std::function<void()>(std::bind(&RobotsTxtRules::initialized, this)));

	const QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (!redirectUrl.isEmpty())
	{
		m_networkAccessor->get(QNetworkRequest(redirectUrl));
		return;
	}

	disconnect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*)));

	const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	const Common::StatusCode statusCodeValue = statusCode.isValid() ?
		static_cast<Common::StatusCode>(statusCode.toInt()) :
		Common::StatusCode::Undefined;

	if (statusCodeValue == Common::StatusCode::Undefined)
	{
		return;
	}

	m_tokenizer->tokenize(reply->readAll());

	m_valid = true;
	m_initialized = true;
}

}
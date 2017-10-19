#include "robots_txt_tokenizer.h"
#include "robots_txt_rules.h"

namespace WebCrawler
{

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
}

RobotsTxtTokenizer::RobotsTxtTokenizer()
	: m_validRobotsTxt(false)
{
}

bool RobotsTxtTokenizer::isValid() const noexcept
{
	return m_validRobotsTxt;
}

void RobotsTxtTokenizer::tokenize(const QString& robotsTxtContent)
{
	QStringList rows = removeCommentaries(robotsTxtContent.split("\n", QString::SkipEmptyParts));

	UserAgentType userAgentType = UserAgentType::AnyBot;

	for (int i = 0; i < rows.size(); ++i)
	{
		auto[token, tokenValue] = tokenizeRow(rows[i]);

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

QList<QString> RobotsTxtTokenizer::tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const
{
	return m_userAgentTokens.contains(userAgentType) ?
		m_userAgentTokens.value(userAgentType).values(token) : QList<QString>();
}

const QUrl& RobotsTxtTokenizer::sitemap() const
{
	return m_sitemapUrl;
}

const QUrl& RobotsTxtTokenizer::originalHostMirror() const
{
	return m_originalHostMirrorUrl;
}

QStringList RobotsTxtTokenizer::removeCommentaries(const QStringList& strings)
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

QPair<QString, QString> RobotsTxtTokenizer::tokenizeRow(const QString& row) const
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

}
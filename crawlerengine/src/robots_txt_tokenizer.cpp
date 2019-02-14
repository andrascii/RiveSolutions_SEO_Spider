#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
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
	static QRegularExpression s_endOfLine(QString("(\\n|\\r|\\r\\n|\\n\\r)"));
	QStringList rows = removeCommentaries(robotsTxtContent.split(s_endOfLine, QString::SkipEmptyParts));

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
			userAgentType = MetaRobotsHelpers::userAgent(tokenValue);
		}
		else
		{
			if (userAgentType == UserAgentType::Unknown)
			{
				continue;
			}

			RobotsTxtToken tokenEnumerator = s_stringToToken.value(token, RobotsTxtToken::TokenUnknown);
			Tokens& tokens = m_userAgentTokens[userAgentType];
			tokens.insert(tokenEnumerator, tokenValue);

			if (tokenEnumerator == RobotsTxtToken::TokenSitemap)
			{
				if (m_sitemapUrl.isValid())
				{
					WARNLOG << "Seems that robots.txt file has several sitemaps";
					continue;
				}

				m_sitemapUrl = tokenValue;
			}
		}
	}

	m_validRobotsTxt = true;
}

QList<QString> RobotsTxtTokenizer::tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const
{
	return m_userAgentTokens.contains(userAgentType) ?
		m_userAgentTokens.value(userAgentType).values(token) : QList<QString>();
}

const Url& RobotsTxtTokenizer::sitemap() const
{
	return m_sitemapUrl;
}

const Url& RobotsTxtTokenizer::originalHostMirror() const
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

bool RobotsTxtTokenizer::hasUserAgentRecord(UserAgentType userAgentType) const
{
	return m_userAgentTokens.contains(userAgentType);
}

QList<RobotsTxtTokenizer::RobotsTxtTokenVauePair> RobotsTxtTokenizer::allowAndDisallowTokens(UserAgentType userAgentType) const
{
	QVector<RobotsTxtTokenVauePair> result;
	const QList<QString> allowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenAllow);
	const QList<QString> disallowTokens = tokenValues(userAgentType, RobotsTxtToken::TokenDisallow);

	foreach(const QString& allowToken, allowTokens)
	{
		result.push_back({ RobotsTxtToken::TokenAllow, allowToken });
	}

	foreach(const QString& disallowToken, disallowTokens)
	{
		result.push_back({ RobotsTxtToken::TokenDisallow, disallowToken });
	}

	auto sortFunc = [](const RobotsTxtTokenVauePair& first, const RobotsTxtTokenVauePair& second)
	{
		if (first.value.size() != second.value.size())
		{
			return first.value.size() < second.value.size();
		}

		return first.token == RobotsTxtToken::TokenAllow && second.token == RobotsTxtToken::TokenDisallow;

	};

	std::sort(result.begin(), result.end(), sortFunc);

	return result.toList();
}

}

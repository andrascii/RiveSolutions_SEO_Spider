#pragma once
#include "stdafx.h"

#include "url.h"

namespace CrawlerEngine
{

enum class UserAgentType;

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

class RobotsTxtTokenizer final
{
public:
	struct RobotsTxtTokenVauePair
	{
		RobotsTxtToken token;
		QString value;
	};

	RobotsTxtTokenizer();

	bool isValid() const noexcept;
	void tokenize(const QString& robotsTxtContent);

	bool hasUserAgentRecord(UserAgentType userAgentType) const;

	QList<QString> tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const;
	const Url& sitemap() const;
	const Url& originalHostMirror() const;

	QList<RobotsTxtTokenVauePair> allowAndDisallowTokens(UserAgentType userAgentType) const;

private:
	QStringList removeCommentaries(const QStringList& strings);
	QPair<QString, QString> tokenizeRow(const QString& row) const;

private:
	using Tokens = QMultiMap<RobotsTxtToken, QString>;

	Url m_sitemapUrl;
	Url m_originalHostMirrorUrl;
	QMap<UserAgentType, Tokens> m_userAgentTokens;
	bool m_validRobotsTxt;
};

}
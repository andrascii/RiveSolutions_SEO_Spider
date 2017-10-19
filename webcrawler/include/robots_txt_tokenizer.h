#pragma once

namespace WebCrawler
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
	RobotsTxtTokenizer();

	bool isValid() const noexcept;
	void tokenize(const QString& robotsTxtContent);

	QList<QString> tokenValues(UserAgentType userAgentType, RobotsTxtToken token) const;
	const QUrl& sitemap() const;
	const QUrl& originalHostMirror() const;

private:
	QStringList removeCommentaries(const QStringList& strings);
	QPair<QString, QString> tokenizeRow(const QString& row) const;

private:
	using Tokens = QMultiMap<RobotsTxtToken, QString>;

	QUrl m_sitemapUrl;
	QUrl m_originalHostMirrorUrl;
	QMap<UserAgentType, Tokens> m_userAgentTokens;
	bool m_validRobotsTxt;
};

}
#include "robots_txt_base_strategy.h"
#include "robots_txt_tokenizer.h"
#include "robots_txt_rules.h"
#include "page_parser_helpers.h"

namespace WebCrawler
{

bool RobotsTxtBaseStrategy::isUrlAllowed(const QUrl& url, 
	UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	if (!tokenizer.isValid())
	{
		return true;
	}

	userAgentType = availableUserAgent(userAgentType, tokenizer);

	if (userAgentType == UserAgentType::InvalidBot)
	{
		return true;
	}

	const QString urlPath = url.path(QUrl::FullyEncoded) + url.query(QUrl::FullyEncoded);

	QList<RobotsTxtTokenizer::RobotsTxtTokenVauePair> tokens = tokenizer.allowAndDisallowTokens(userAgentType);

	bool allowed = true;
	foreach(const RobotsTxtTokenizer::RobotsTxtTokenVauePair& token, tokens)
	{
		if (patternMatched(token.value, urlPath))
		{
			allowed = token.token == RobotsTxtToken::TokenAllow ? true : false;
		}
	}

	return allowed;
}

bool RobotsTxtBaseStrategy::patternMatched(const QString& pattern, const QString value) const
{
	if (!pattern.contains(QString("*")) && !pattern.contains(QString("$")))
	{
		return value.startsWith(pattern);
	}

	ASSERT(!"Not implemented yet");
	return false;
}

UserAgentType RobotsTxtBaseStrategy::availableUserAgent(UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	if (tokenizer.hasUserAgentRecord(userAgentType))
	{
		return userAgentType;
	}

	return tokenizer.hasUserAgentRecord(UserAgentType::AnyBot) ? UserAgentType::AnyBot : UserAgentType::InvalidBot;
}

}

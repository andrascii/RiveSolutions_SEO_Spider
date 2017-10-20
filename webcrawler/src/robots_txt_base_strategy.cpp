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

	if (pattern.contains(QString("$")) && pattern.indexOf("$") != pattern.size() - 1)
	{
		// bad pattern
		return false;
	}

	QStringList parts = pattern.split(QString("*"), QString::SkipEmptyParts);
	int index = 0;
	bool matched = true;

	for (int i = 0; i < parts.size(); i++)
	{
		bool lastPart = i == parts.size() - 1;
		bool strongMatch = false;

		if (lastPart && parts[i].endsWith("$"))
		{
			strongMatch = true;
			ASSERT(!strongMatch || lastPart);
			parts[i] = parts[i].mid(0, parts[i].size() - 1);
		}


		if (i == 0 || pattern.startsWith(QString("*")))
		{
			matched = strongMatch ? value.endsWith(parts[i]) : value.indexOf(parts[i]) != -1;
			if (!matched)
			{
				break;
			}

			if (!strongMatch)
			{
				index = value.indexOf(parts[i]) + parts[i].size();
			}
			continue;
		}


		const int matchedIndex = value.indexOf(parts[i], index);
		matched = strongMatch ? matchedIndex + parts[i].size() == pattern.size() - 1 : matchedIndex != -1;

		if (!matched)
		{
			break;
		}

		index = matchedIndex + parts[i].size();
	}

	return matched;
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

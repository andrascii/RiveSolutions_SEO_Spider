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

	if (!checkIfSupportedRecordExistsAndCorrectUserAgentType(userAgentType, tokenizer))
	{
		return true;
	}
	
	QUrl cleanedUrl = cleanUrl(url, userAgentType, tokenizer);

	const QString urlPath = cleanedUrl.path(QUrl::FullyEncoded) + cleanedUrl.query(QUrl::FullyEncoded);

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

QUrl RobotsTxtBaseStrategy::cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	Q_UNUSED(userAgentType);
	Q_UNUSED(tokenizer);
	return url;
}

bool RobotsTxtBaseStrategy::patternMatched(const QString& pattern, const QString& value) const
{
	QString cannonicalPattern = pattern.toLower();
	QString cannonicalValue = QUrl(QUrl(value).toDisplayString().toLower()).toDisplayString(QUrl::FullyEncoded);

	if (!cannonicalPattern.contains(QString("*")) && !cannonicalPattern.contains(QString("$")))
	{
		return cannonicalValue.toLower().startsWith(cannonicalPattern.toLower());
	}

	if (cannonicalPattern.contains(QString("$")) && cannonicalPattern.indexOf("$") != cannonicalPattern.size() - 1)
	{
		// bad pattern
		return false;
	}

	QStringList parts = cannonicalPattern.split(QString("*"), QString::SkipEmptyParts);
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


		if (i == 0 || cannonicalPattern.startsWith(QString("*")))
		{
			matched = strongMatch ? cannonicalValue.endsWith(parts[i]) : cannonicalValue.indexOf(parts[i]) != -1;
			if (!matched)
			{
				break;
			}

			if (!strongMatch)
			{
				index = cannonicalValue.indexOf(parts[i]) + parts[i].size();
			}
			continue;
		}


		const int matchedIndex = cannonicalValue.indexOf(parts[i], index);
		matched = strongMatch ? matchedIndex + parts[i].size() == cannonicalPattern.size() - 1 : matchedIndex != -1;

		if (!matched)
		{
			break;
		}

		index = matchedIndex + parts[i].size();
	}

	return matched;
}

bool RobotsTxtBaseStrategy::checkIfSupportedRecordExistsAndCorrectUserAgentType(UserAgentType& userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	if (tokenizer.hasUserAgentRecord(userAgentType))
	{
		return true;
	}

	if (tokenizer.hasUserAgentRecord(UserAgentType::AnyBot))
	{
		userAgentType = UserAgentType::AnyBot;
		return true;
	}

	return false;
}

}

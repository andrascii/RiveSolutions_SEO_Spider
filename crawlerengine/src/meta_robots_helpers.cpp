#include "meta_robots_helpers.h"
#include "robots_txt_tokenizer.h"

namespace CrawlerEngine
{

namespace
{

const QMap<QString, UserAgentType> s_userAgent =
{
	{ "googlebot", UserAgentType::GoogleBot },
	{ "yandex", UserAgentType::YandexBot },
	{ "mail.ru", UserAgentType::MailRuBot },
	{ "msnbot", UserAgentType::MsnBot },
	{ "slurp", UserAgentType::YahooBot },
	{ "*", UserAgentType::AnyBot }
};

}

UserAgentType MetaRobotsHelpers::userAgent(const QString& userAgentStr)
{
	const QString fixedUserAgentStr = userAgentStr == QString("robots") ? QString("*") : userAgentStr;
	return s_userAgent.value(fixedUserAgentStr.toLower().trimmed(), UserAgentType::Unknown);
}

QStringList MetaRobotsHelpers::supportedUserAgents(bool metaRobots)
{
	QStringList result = s_userAgent.keys();

	if (metaRobots)
	{
		result.removeOne(QString("*"));
		result.push_back(QString("robots"));
	}

	return result;
}

bool MetaRobotsHelpers::checkIfSupportedRecordExistsAndCorrectUserAgentType(UserAgentType& userAgentType, const RobotsTxtTokenizer& tokenizer)
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

bool MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(UserAgentType& userAgentType,
	const MetaRobotsFlagsSet& metaRobotsFlags)
{
	const auto strategyIter = metaRobotsFlags.find(userAgentType);

	if (strategyIter != metaRobotsFlags.end() && strategyIter->second)
	{
		return true;
	}

	if (metaRobotsFlags.find(UserAgentType::AnyBot) != metaRobotsFlags.end())
	{
		userAgentType = UserAgentType::AnyBot;
		return true;
	}

	return false;
}

}

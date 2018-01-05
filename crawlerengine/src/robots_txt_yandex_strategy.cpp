#include "robots_txt_yandex_strategy.h"
#include "robots_txt_tokenizer.h"
#include "meta_robots_helpers.h"


namespace CrawlerEngine
{

Url RobotsTxtYandexStrategy::cleanUrl(const Url& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	if (!MetaRobotsHelpers::checkIfSupportedRecordExistsAndCorrectUserAgentType(userAgentType, tokenizer))
	{
		return url;
	}

	QList<QString> cleanParamTokens = tokenizer.tokenValues(userAgentType, RobotsTxtToken::TokenCleanParam);
	if (cleanParamTokens.isEmpty())
	{
		return url;
	}

	QString query = url.query(Url::FullyEncoded);
	QString path = url.path(Url::FullyEncoded);

	QStringList params = query.split(QString("&"), QString::SkipEmptyParts);
	QMap<QString, QString> paramValuesByName;
	foreach(const QString& param, params)
	{
		QStringList paramParts = param.split(QString("="));
		if (paramParts.size() == 1)
		{
			paramValuesByName[param] = QString();
			continue;
		}

		paramValuesByName[paramParts.front().toLower()] = paramParts.back();
	}


	foreach(const QString& cleanParamToken, cleanParamTokens)
	{
		QStringList parts = cleanParamToken.split(QRegExp(QString("\\s")), QString::SkipEmptyParts);
		if (parts.empty() || parts.size() > 2)
		{
			continue;
		}

		if (parts.size() == 2 && !parts.back().isEmpty())
		{
			if (!patternMatched(parts.back(), path + query))
			{
				continue;
			}
		}

		QStringList paramsToClean = parts.front().split(QString("&"), QString::SkipEmptyParts);

		foreach(const QString& paramToClean, paramsToClean)
		{
			paramValuesByName.remove(paramToClean.toLower());
		}
	}

	query = QString();
	bool first = true;
	foreach(const QString& param, params)
	{
		QStringList paramParts = param.split(QString("="));
		if (paramValuesByName.find(paramParts.front().toLower()) != paramValuesByName.end())
		{
			if (!first)
			{
				query += QString("&");
			}

			query += param;
			first = false;
		}
	}

	Url result = url;
	result.setQuery(query);
	return result;
}

bool RobotsTxtYandexStrategy::isUrlAllowed(const MetaRobotsFlagsSet& metaRobotsFlags, UserAgentType userAgentType) const
{
	if (!MetaRobotsHelpers::checkIfSupportedMetaRobotsExistAndCorrectUserAgentType(userAgentType, metaRobotsFlags))
	{
		return true;
	}

	const MetaRobotsFlags& flags = metaRobotsFlags.find(userAgentType)->second;

	return flags.testFlag(MetaRobotsAll) ||
		flags.testFlag(MetaRobotsFollow) ||
		(!flags.testFlag(MetaRobotsNone) &&
		!flags.testFlag(MetaRobotsNoFollow));
}

}


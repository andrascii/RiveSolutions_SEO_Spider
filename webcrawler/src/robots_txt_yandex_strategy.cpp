#include "robots_txt_yandex_strategy.h"
#include "robots_txt_tokenizer.h"


namespace WebCrawler
{

QUrl RobotsTxtYandexStrategy::cleanUrl(const QUrl& url, UserAgentType userAgentType, const RobotsTxtTokenizer& tokenizer) const
{
	if (!checkIfSupportedRecordExistsAndCorrectUserAgentType(userAgentType, tokenizer))
	{
		return url;
	}

	QList<QString> cleanParamTokens = tokenizer.tokenValues(userAgentType, RobotsTxtToken::TokenCleanParam);
	if (cleanParamTokens.isEmpty())
	{
		return url;
	}

	QString query = url.query(QUrl::FullyEncoded);
	QString path = url.path(QUrl::FullyEncoded);

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

	QUrl result = url;
	result.setQuery(query);
	return result;
}

}


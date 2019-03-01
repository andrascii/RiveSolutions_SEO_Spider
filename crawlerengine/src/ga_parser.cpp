#include "ga_parser.h"
#include "ihtml_node.h"
#include "ihtml_parser.h"

namespace
{
// https://developers.google.com/analytics/devguides/collection/gajs/
// _gaq.push(['_setAccount', 'UA-XXXXX-X']);
QString oldGaInitializer = QString("_setAccount");
QString oldGaInitializerEnd = QString("]");
QRegularExpression oldGaIdentifierRegExp(",\\s*['|\"](UA-.{3,6}-.)['|\"]\\s*]", QRegularExpression::MultilineOption);

// var pageTracker = _gat._getTracker('UA-XXXXX-X');
QString oldGaInitializer2 = QString("_getTracker");
QString oldGaInitializerEnd2 = QString(")");
QRegularExpression oldGaIdentifierRegExp2("\\s*\\(\\s*['|\"](UA-.{3,6}-.)['|\"]\\s*\\)", QRegularExpression::MultilineOption);

// var pageTracker = _gat._createTracker('UA-XXXXX-X', 'myTracker');
QString oldGaInitializer3 = QString("_createTracker");
QString oldGaInitializerEnd3 = QString(")");
QRegularExpression oldGaIdentifierRegExp3("\\s*\\(\\s*['|\"](UA-.{3,6}-.)['|\"]\\s*,\\s*['|\"].*['|\"]\\s*\\)", QRegularExpression::MultilineOption);

struct Pattern
{
	QString initializer;
	QString initializerEnd;
	QRegularExpression pattern;
};

bool patternMatched(const Pattern& pattern, const QString& javaScriptCode, const QString& counterNumber)
{
	int index = 0;
	while (javaScriptCode.indexOf(pattern.initializer, index) != -1)
	{
		const int gaStartIndex = javaScriptCode.indexOf(pattern.initializer, index);
		if (gaStartIndex == -1)
		{
			return false;
		}

		const int searchBeginIndex = gaStartIndex;
		int searchEndIndex = javaScriptCode.indexOf(pattern.initializerEnd, searchBeginIndex + pattern.initializer.size());

		if (searchEndIndex == -1)
		{
			return false;
		}

		searchEndIndex += pattern.initializerEnd.size();

		const QString content = javaScriptCode.mid(searchBeginIndex, searchEndIndex - searchBeginIndex);
		const QRegularExpressionMatch match = pattern.pattern.match(content);

		if (match.isValid() && match.hasMatch())
		{
			const QString foundId = match.captured(1);
			const bool result = foundId == counterNumber;
			if (result)
			{
				return true;
			}
		}

		index = searchEndIndex;
	}

	return false;
}

bool findOldGaCounter(const QString& javaScriptCode, const QString& counterNumber)
{
	static std::vector<Pattern> s_patterns =
	{
		{ oldGaInitializer, oldGaInitializerEnd, oldGaIdentifierRegExp },
		{ oldGaInitializer2, oldGaInitializerEnd2, oldGaIdentifierRegExp2 },
		{ oldGaInitializer3, oldGaInitializerEnd3, oldGaIdentifierRegExp3 },
	};

	for (const auto& pattern : s_patterns)
	{
		if (patternMatched(pattern, javaScriptCode, counterNumber))
		{
			return true;
		}
	}

	return false;
}

// window['ga-disable-UA-XXXXXX-Y'] = true;
bool findOldGaDisabledCounter(const QString& javaScriptCode, const QString& counterNumber)
{
	static Pattern s_oldGaDisablePattern =
	{
		QString("window"),
		QString("true"),
		QRegularExpression("\\s*['|\"]ga-disable-(UA-.{3,6}-.)['|\"]\\s*]\\s*=\\s*true", QRegularExpression::MultilineOption)
	};

	return patternMatched(s_oldGaDisablePattern, javaScriptCode, counterNumber);
}

// ga('create', 'UA-XXXXX-Y', 'auto');
bool findGaCounter(const QString& javaScriptCode, const QString& counterNumber)
{
	static Pattern s_gaPattern =
	{
		QString("ga"),
		QString(")"),
		QRegularExpression("ga\\s*\\(\\s*['|\"]\\s*create['|\"]\\s*,\\s*['|\"](UA-.{3,6}-.)['|\"]\\s*(,\\s*['|\"]\\w*['|\"]\\s*)?\\)", QRegularExpression::MultilineOption)
	};

	return patternMatched(s_gaPattern, javaScriptCode, counterNumber);
}

// gtag('config', 'GA_TRACKING_ID', { 'send_page_view': false });
bool findGtagCounter(const QString& javaScriptCode, const QString& counterNumber)
{
	static Pattern s_gtagPattern =
	{
		QString("gtag"),
		QString(")"),
		QRegularExpression("gtag\\s*\\(\\s*['|\"]config['|\"]\\s*,\\s*['|\"](\\w*)['|\"]\\s*(,\\s*{\\s*['|\"]\\w*['|\"]\\s*:\\s*\\w*\\s*}\\s*)?\\)", QRegularExpression::MultilineOption)
	};

	return patternMatched(s_gtagPattern, javaScriptCode, counterNumber);
}

}

namespace CrawlerEngine
{
GaParser::GaParser(IHtmlParser* htmlParser, const QString& counterNumber, StorageType targetStorageType)
	: m_htmlParser(htmlParser)
	, m_counterNumber(counterNumber)
	, m_targetStorageType(targetStorageType)
{
}

void GaParser::parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage)
{
	Q_UNUSED(headers);
	if (parsedPage->resourceType != ResourceType::ResourceHtml || parsedPage->isThisExternalPage)
	{
		return;
	}

	const std::vector<IHtmlNodeCountedPtr> tagScripts = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdScript);

	const auto gaIter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findGaCounter(node->text(), m_counterNumber) || findGtagCounter(node->text(), m_counterNumber);
	});

	if (gaIter != tagScripts.end())
	{
		return;
	}

	// if there is no GA or GTAG counter try to find old GA counter

	const auto iter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findOldGaCounter(node->text(), m_counterNumber);
	});

	bool idIsFound = iter != tagScripts.end();

	if (idIsFound)
	{
		const auto disabledIter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
		{
			return findOldGaDisabledCounter(node->text(), this->m_counterNumber);
		});

		const bool disableIdIsFound = disabledIter != tagScripts.end();
		if (disableIdIsFound)
		{
			idIsFound = false;
		}
	}

	if (!idIsFound)
	{
		parsedPage->missingGoogleAnalyticsCounters.push_back(m_targetStorageType);
	}
}

}

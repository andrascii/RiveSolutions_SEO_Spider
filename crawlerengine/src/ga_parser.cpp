#include "ga_parser.h"
#include "ihtml_node.h"
#include "ihtml_parser.h"

namespace
{
// https://developers.google.com/analytics/devguides/collection/gajs/
// _gaq.push(['_setAccount', 'UA-XXXXX-X']);
QString oldGaInitializer = QString("_setAccount");
QString oldGaInitializerEnd = QString("]");
QRegularExpression oldGaIdentifierRegExp(",\\s*['|\"](UA-.{3,5}-.)['|\"]", QRegularExpression::MultilineOption);
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

	const auto iter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findGaCounter(node->text()) || findGaCounter2(node->text());
	});

	if (iter == tagScripts.end())
	{
		parsedPage->missingGoogleAnalyticsCounters.push_back(m_targetStorageType);
	}
}

bool GaParser::findGaCounter(const QString& javaScriptCode) const
{
	int index = 0;
	while (javaScriptCode.indexOf(oldGaInitializer, index) != -1)
	{
		const int gaStartIndex = javaScriptCode.indexOf(oldGaInitializer, index);
		if (gaStartIndex == -1)
		{
			return false;
		}

		const int searchBeginIndex = gaStartIndex + oldGaInitializer.size();
		const int searchEndIndex = javaScriptCode.indexOf(oldGaInitializerEnd, searchBeginIndex);

		if (searchEndIndex == -1)
		{
			return false;
		}

		const QString content = javaScriptCode.mid(searchBeginIndex, searchEndIndex - searchBeginIndex);
		const QRegularExpressionMatch match = oldGaIdentifierRegExp.match(content);

		if (!match.isValid() || !match.hasMatch())
		{
			return false;
		}

		const QString foundId = match.captured(1);
		const bool result = foundId == m_counterNumber;
		if (result)
		{
			return true;
		}
		index = searchEndIndex;
	}
	return false;
}

bool GaParser::findGaCounter2(const QString& javaScriptCode) const
{
	Q_UNUSED(javaScriptCode);
	return false;
}

}

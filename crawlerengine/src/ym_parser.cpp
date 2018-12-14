#include "ym_parser.h"
#include "ihtml_node.h"
#include "ihtml_parser.h"

namespace
{
QString oldYaMetrikaInitializer = QString("Ya.Metrika2");
QString oldYaMetrikaInitializerEnd = QString("}");
QRegularExpression oldYmIdentifierRegExp("id\\s*:\\s*(\\d+)\\W", QRegularExpression::MultilineOption);

QString newYaMetrikaInitializer = QString("ym");
QString newYaMetrikaInitializerEnd = QString("init");
QRegularExpression newYmIdentifierRegExp("ym\\s*\\(\\s*(\\d+)\\s*,\\s*['|\"]init['|\"]", QRegularExpression::MultilineOption);

}

namespace CrawlerEngine
{

YmParser::YmParser(IHtmlParser* htmlParser, int counterNumber, StorageType targetStorageType)
	: m_htmlParser(htmlParser)
	, m_counterNumber(counterNumber)
	, m_targetStorageType(targetStorageType)
{
}

void YmParser::parse(const ResponseHeaders&, ParsedPagePtr& parsedPage)
{
	if (parsedPage->resourceType != ResourceType::ResourceHtml || parsedPage->isThisExternalPage)
	{
		return;
	}

	const std::vector<IHtmlNodeCountedPtr> tagScripts = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdScript);

	const auto iter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findYaCounter(node->text()) || findYaCounter2(node->text()) || findYaCounter3(node->text());
	});

	if (iter == tagScripts.end())
	{
		parsedPage->missingYandexMetricaCounters.push_back(m_targetStorageType);
	}
}

bool YmParser::findYaCounter(const QString& javaScriptCode) const
{
	// TODO: move constanst outside of the function
	const QString ymIdentifier = QString("w.yaCounter%1").arg(m_counterNumber);
	const int index = javaScriptCode.indexOf(ymIdentifier);
	if (index == -1)
	{
		return false;
	}

	const int nextCharIndex = index + ymIdentifier.size();
	if (nextCharIndex == javaScriptCode.size())
	{
		return true;
	}

	const QChar& nextSymbol = javaScriptCode.at(nextCharIndex);
	return nextSymbol < '0' || nextSymbol > '9';
}

bool YmParser::findYaCounter2(const QString& javaScriptCode) const
{
	int index = 0;
	while (javaScriptCode.indexOf(oldYaMetrikaInitializer, index) != -1)
	{
		const int ymStartIndex = javaScriptCode.indexOf(oldYaMetrikaInitializer, index);
		if (ymStartIndex == -1)
		{
			return false;
		}

		const int searchBeginIndex = ymStartIndex + oldYaMetrikaInitializer.size();
		const int searchEndIndex = javaScriptCode.indexOf(oldYaMetrikaInitializerEnd, searchBeginIndex);

		if (searchEndIndex == -1)
		{
			return false;
		}

		const QString content = javaScriptCode.mid(searchBeginIndex, searchEndIndex - searchBeginIndex);
		const QRegularExpressionMatch match = oldYmIdentifierRegExp.match(content);

		if (!match.isValid() || !match.hasMatch())
		{
			return false;
		}

		const int foundId = match.captured(1).toInt();
		const bool result = foundId == m_counterNumber;
		if (result)
		{
			return true;
		}
		index = searchEndIndex;
	}
	return false;
}

bool YmParser::findYaCounter3(const QString& javaScriptCode) const
{
	int index = 0;
	while (javaScriptCode.indexOf(newYaMetrikaInitializer, index) != -1)
	{
		const int ymStartIndex = javaScriptCode.indexOf(newYaMetrikaInitializer, index);
		if (ymStartIndex == -1)
		{
			return false;
		}

		const int searchBeginIndex = ymStartIndex;
		int searchEndIndex = javaScriptCode.indexOf(newYaMetrikaInitializerEnd, searchBeginIndex);

		if (searchEndIndex == -1)
		{
			return false;
		}

		searchEndIndex += newYaMetrikaInitializerEnd.size() + 1;

		const QString content = javaScriptCode.mid(searchBeginIndex, searchEndIndex - searchBeginIndex);
		const QRegularExpressionMatch match = newYmIdentifierRegExp.match(content);

		if (!match.isValid() || !match.hasMatch())
		{
			return false;
		}

		const int foundId = match.captured(1).toInt();
		const bool result = foundId == m_counterNumber;
		if (result)
		{
			return true;
		}
		index = searchEndIndex;
	}
	return false;
}
}

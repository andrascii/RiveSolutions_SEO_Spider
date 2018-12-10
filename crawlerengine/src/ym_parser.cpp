#include "ym_parser.h"
#include "ihtml_node.h"
#include "ihtml_parser.h"

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
	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	const std::vector<IHtmlNodeCountedPtr> tagScripts = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdScript);

	const auto iter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findYaCounter(node->text());
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
	if (nextCharIndex == ymIdentifier.size())
	{
		return true;
	}

	const QChar& nextSymbol = javaScriptCode.at(nextCharIndex);
	return nextSymbol < 48 || nextSymbol > 57; // 48='0', 57='9'
}

}
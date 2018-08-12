#include "ym_parser.h"
#include "ihtml_node.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

YmParser::YmParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
}

void YmParser::parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage)
{
	headers;

	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	const std::vector<IHtmlNodeCountedPtr> headTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdHead);

	if (headTags.empty())
	{
		return;
	}

	std::vector<IHtmlNodeCountedPtr> tagScripts = headTags[0]->matchSubNodes(IHtmlNode::TagIdScript);

	const auto iter = std::find_if(tagScripts.begin(), tagScripts.end(), [this](const IHtmlNodeCountedPtr& node)
	{
		return findYaCounter(node->text());
	});

	parsedPage->pageCounterIds |= static_cast<std::uint32_t>(PageCounterId::YandexMetrica);
}

bool YmParser::findYaCounter(const QString& javaScriptCode) const
{
	return javaScriptCode.contains("w.yaCounter");
}

}
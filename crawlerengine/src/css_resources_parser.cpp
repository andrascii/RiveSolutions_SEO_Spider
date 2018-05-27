#include "css_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

CssResourcesParser::CssResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceStyleSheet));
}

void CssResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage)
{
	if (parsedPage->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	std::vector<IHtmlNodeSharedPtr> linkTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdLink);

	const auto isBadLinkTag = [](const IHtmlNodeSharedPtr& linkTag)
	{
		if (linkTag->type() != IHtmlNode::NodeTypeElement)
		{
			return true;
		}

		const bool hasStyleSheetRelAttribute = linkTag->hasAttribute("rel") &&
			linkTag->attribute("rel").toLower().trimmed() == "stylesheet";

		return !linkTag->hasAttribute("href") && !hasStyleSheetRelAttribute;
	};

	linkTags.erase(std::remove_if(linkTags.begin(), linkTags.end(), isBadLinkTag), linkTags.end());

	std::vector<Url> linksHrefValues;
	linksHrefValues.reserve(linkTags.size());

	for (const IHtmlNodeSharedPtr& linkTag : linkTags)
	{
		linksHrefValues.emplace_back(linkTag->attribute("href").trimmed().remove(m_regExp));
	}

	PageParserHelpers::resolveUrlList(parsedPage->baseUrl, linksHrefValues);

	for (const Url& url : linksHrefValues)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage cssResource
		{
			ResourceType::ResourceStyleSheet,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (parsedPage->allResourcesOnPage.find(cssResource) == parsedPage->allResourcesOnPage.end())
		{
			parsedPage->allResourcesOnPage.insert(std::move(cssResource));
		}
	}

	CompoundParser::parse(headers, parsedPage);
}

}

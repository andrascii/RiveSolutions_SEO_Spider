#include "js_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

JsResourcesParser::JsResourcesParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceJavaScript));
}

void JsResourcesParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

// 	auto predicate = [](const GumboNode* node)
// 	{
// 		return node &&
// 			node->type == GUMBO_NODE_ELEMENT &&
// 			node->v.element.tag == GUMBO_TAG_SCRIPT &&
// 			gumbo_get_attribute(&node->v.element.attributes, "src");
// 	};
// 
// 	auto resultGetter = [](const GumboNode* node)
// 	{
// 		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
// 		return Url(src->value);
// 	};
// 
// 	DEBUG_ASSERT(page->baseUrl.isValid());
// 
// 	std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);
// 	std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->baseUrl, urls);
// 
// 	for (const Url& url : resolvedUrls)
// 	{
// 		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));
// 
// 		const ResourceOnPage jsResource
// 		{
// 			ResourceType::ResourceJavaScript,
// 			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
// 		};
// 
// 		if (page->allResourcesOnPage.find(jsResource) == page->allResourcesOnPage.end())
// 		{
// 			page->allResourcesOnPage.insert(std::move(jsResource));
// 		}
// 	}
// 
// 	CompoundParser::parse(output, headers, page);

	//////////////////////////////////////////////////////////////////////////

	std::vector<IHtmlNodeSharedPtr> scriptTags = m_htmlParser->matchNodesInDepth(IHtmlNode::TagIdScript);

	const auto isBadScriptTag = [](const IHtmlNodeSharedPtr& scriptTag)
	{
		return !scriptTag->hasAttribute("src");
	};

	scriptTags.erase(std::remove_if(scriptTags.begin(), scriptTags.end(), isBadScriptTag), scriptTags.end());

	std::vector<Url> scriptSrcValues;
	scriptSrcValues.reserve(scriptTags.size());

	for (const IHtmlNodeSharedPtr& scriptTag : scriptTags)
	{
		scriptSrcValues.emplace_back(scriptTag->attribute("src").trimmed().remove(m_regExp));
	}

	const std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->baseUrl, scriptSrcValues);

	for (const Url& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage jsResource
		{
			ResourceType::ResourceJavaScript,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (page->allResourcesOnPage.find(jsResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(jsResource));
		}
	}

	CompoundParser::parse(headers, page);
}

}

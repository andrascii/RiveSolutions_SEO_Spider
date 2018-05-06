#include "js_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

JsResourcesParser::JsResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceJavaScript));
}

void JsResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_SCRIPT &&
			gumbo_get_attribute(&node->v.element.attributes, "src");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return Url(src->value);
	};

	DEBUG_ASSERT(page->baseUrl.isValid());

	std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->baseUrl, urls);

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

	CompoundParser::parse(output, headers, page);
}

}

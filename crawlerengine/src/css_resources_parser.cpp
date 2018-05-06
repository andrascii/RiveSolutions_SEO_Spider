#include "css_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

CssResourcesParser::CssResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceStyleSheet));
}

void CssResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto predicate = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_LINK &&
			gumbo_get_attribute(&node->v.element.attributes, "href") &&
			GumboParsingHelpers::checkAttribute(node, "rel", "stylesheet");
	};

	auto resultGetter = [](const GumboNode* node)
	{
		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		return Url(href->value);
	};

	DEBUG_ASSERT(page->baseUrl.isValid());

	const std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);
	const std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->baseUrl, urls);

	for (const Url& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage cssResource
		{
			ResourceType::ResourceStyleSheet,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		if (page->allResourcesOnPage.find(cssResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(cssResource));
		}
	}

	CompoundParser::parse(output, headers, page);
}

}

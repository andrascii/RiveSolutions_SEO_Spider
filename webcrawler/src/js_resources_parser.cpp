#include "js_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

JsResourcesParser::JsResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceJavaScript));
}

void JsResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
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
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
// 		if (m_resourcesCache->isResourceExists(url))
// 		{
// 			continue;
// 		}

		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const RawResourceOnPage jsResource
		{
			ResourceType::ResourceJavaScript,
			LinkInfo{ url, LinkParameter::UnknownParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.insert(jsResource);
	}

	CompoundParser::parse(output, page);
}

}

#include "css_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

CssResourcesParser::CssResourcesParser(ResourcesCache* resourcesCache)
	: m_resourcesCache(resourcesCache)
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceStyleSheet, resourcesCache));
}

void CssResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_LINK &&
			gumbo_get_attribute(&node->v.element.attributes, "href") &&
			GumboParsingHelpers::checkAttribute(node, "rel", "stylesheet");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		return QUrl(href->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (m_resourcesCache->isResourceExists(url))
		{
			continue;
		}

		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		RawResourceOnPage cssResource
		{
			ResourceType::ResourceStyleSheet,
			LinkInfo{ url, LinkParameter::UnknownParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.push_back(cssResource);
	}

	CompoundParser::parse(output, page);
}

void CssResourcesParser::init()
{
	m_resourcesCache->clear();
}

}

#include "css_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"

namespace WebCrawler
{

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

	auto checkResourceExistence = [&page](const QUrl& url) -> bool
	{
		auto pred = [&url](const RawResourceOnPage& resource) -> bool
		{
			return resource.thisResourceLink.url == url;
		};

		return std::any_of(page->allResourcesOnPage.begin(), page->allResourcesOnPage.end(), pred);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (checkResourceExistence(url))
		{
			continue;
		}

		RawResourceOnPage cssResource
		{
			ResourceType::ResourceStyleSheet,
			LinkInfo{ url, LinkParameter::UnknownParameter }
		};

		page->allResourcesOnPage.push_back(cssResource);
	}
}

}
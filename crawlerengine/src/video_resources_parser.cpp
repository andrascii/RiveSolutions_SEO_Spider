#include "video_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

VideoResourcesParser::VideoResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceVideo));
}

void VideoResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_VIDEO &&
			(!!GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", "")) ||
				gumbo_get_attribute(&node->v.element.attributes, "src"));
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* child = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));

		GumboAttribute* src = child != nullptr
			? gumbo_get_attribute(&child->v.element.attributes, "src")
			: gumbo_get_attribute(&node->v.element.attributes, "src");
		
		return Url(src->value);
	};

	std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<Url> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const Url& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage videoResource
		{
			ResourceType::ResourceVideo,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.emplace_back(std::move(videoResource));
	}

	CompoundParser::parse(output, headers, page);
}

}

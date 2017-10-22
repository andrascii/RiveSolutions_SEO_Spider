#include "video_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

VideoResourcesParser::VideoResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceVideo));
}

void VideoResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
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
			!!PageParserHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* child = PageParserHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
		GumboAttribute* src = gumbo_get_attribute(&child->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = PageParserHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const RawResourceOnPage videoResource
		{
			ResourceType::ResourceVideo,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.insert(videoResource);
	}

	CompoundParser::parse(output, page);
}

}

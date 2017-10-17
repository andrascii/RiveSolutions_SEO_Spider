#include "video_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

VideoResourcesParser::VideoResourcesParser(ResourcesCache* resourcesCache)
	: m_resourcesCache(resourcesCache)
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceVideo, resourcesCache));
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
			!!GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* child = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_SOURCE, std::make_pair("src", ""));
		GumboAttribute* src = gumbo_get_attribute(&child->v.element.attributes, "src");
		return QUrl(src->value);
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

		RawResourceOnPage videoResource
		{
			ResourceType::ResourceVideo,
			LinkInfo{ url, LinkParameter::UnknownParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.push_back(videoResource);
	}

	CompoundParser::parse(output, page);
}

void VideoResourcesParser::init()
{
	m_resourcesCache->clear();
}

}

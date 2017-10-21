#include "images_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

ImagesResourcesParser::ImagesResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceImage));
}

void ImagesResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_IMG &&
			gumbo_get_attribute(&node->v.element.attributes, "src");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		GumboAttribute* alt = gumbo_get_attribute(&node->v.element.attributes, "alt");
		QString altVal = alt == nullptr ? "" : alt->value;
		return std::make_pair(QUrl(src->value), altVal);
	};

	std::vector<std::pair<QUrl, QString>> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	for (std::pair<QUrl, QString>& url : urls)
	{
		url.first = PageParserHelpers::resolveUrl(page->url, url.first);
// 		if (m_resourcesCache->isResourceExists(url.first))
// 		{
// 			continue;
// 		}

		const bool dataResource = url.first.toDisplayString().startsWith(QString("data:"));

		RawResourceOnPage imageResource
		{
			ResourceType::ResourceImage,
			LinkInfo{ url.first, LinkParameter::UnknownParameter, url.second, dataResource },
			ResourceSource::SourceTagImg,
		};

		page->allResourcesOnPage.insert(imageResource);
	}

	CompoundParser::parse(output, page);
}

}

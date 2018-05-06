#include "images_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

ImagesResourcesParser::ImagesResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceImage));
}

void ImagesResourcesParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto predicate = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_IMG &&
			gumbo_get_attribute(&node->v.element.attributes, "src");
	};

	auto resultGetter = [](const GumboNode* node)
	{
		const QRegularExpression regExp("[\\n\\t]+");

		const GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		const GumboAttribute* alt = gumbo_get_attribute(&node->v.element.attributes, "alt");

		const QString altVal = alt == nullptr ? QString() : alt->value;
		const Url url = QString(src->value).remove(regExp);

		return std::make_pair(url, altVal);
	};

	std::vector<std::pair<Url, QString>> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);

	for (std::pair<Url, QString>& url : urls)
	{
		url.first = PageParserHelpers::resolveUrl(page->baseUrl, url.first);

		const bool dataResource = url.first.toDisplayString().startsWith(QString("data:"));

		const ResourceOnPage imageResource
		{
			ResourceType::ResourceImage,
			LinkInfo{ url.first, LinkParameter::DofollowParameter, url.second, dataResource, ResourceSource::SourceTagImg }
		};

		if (page->allResourcesOnPage.find(imageResource) == page->allResourcesOnPage.end())
		{
			page->allResourcesOnPage.insert(std::move(imageResource));
		}
	}

	CompoundParser::parse(output, headers, page);
}

}

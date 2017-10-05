#include "flash_resources_parser.h"
#include "gumbo_parsing_helpers.h"
#include "page_parser_helpers.h"
#include "resources_cache.h"

namespace WebCrawler
{

FlashResourcesParser::FlashResourcesParser(ResourcesCache* resourcesCache)
	: m_resourcesCache(resourcesCache)
{

}

void FlashResourcesParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseFlashResourcesV1(output, page);
	parseFlashResourcesV2(output, page);
	parseFlashResourcesV3(output, page);
}

void FlashResourcesParser::init()
{
	m_resourcesCache->clear();
}

void FlashResourcesParser::parseFlashResourcesV1(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	//<embed type="application/x-shockwave-flash" src="myContent.swf" width="300" height="120" pluginspage="http://www.adobe.com/go/getflashplayer" / >
	//<noembed>Alternative content</noembed>

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_EMBED &&
			gumbo_get_attribute(&node->v.element.attributes, "src") &&
			GumboParsingHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
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
		if (isResourceExists(url))
		{
			continue;
		}

		RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::UnknownParameter }
		};

		page->allResourcesOnPage.push_back(flashResource);
	}
}

void FlashResourcesParser::parseFlashResourcesV2(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	// 	<object type="application/x-shockwave-flash" data="myContent.swf" width="300" height="120">
	// 	<p>Alternative content</p>
	// 	</object>

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_OBJECT &&
			gumbo_get_attribute(&node->v.element.attributes, "data") &&
			GumboParsingHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "data");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (isResourceExists(url))
		{
			continue;
		}

		RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::UnknownParameter }
		};

		page->allResourcesOnPage.push_back(flashResource);
	}
}

void FlashResourcesParser::parseFlashResourcesV3(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	// 	<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="300" height="120">
	// 	<param name="movie" value="myContent.swf" />
	// 	<p>Alternative content</p>
	// 	</object>

	auto cond = [](const GumboNode* node)
	{
		const bool paramTagContainsMovieOrSrcAttribute =
			GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", "")) ||
			GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", ""));

		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_OBJECT &&
			GumboParsingHelpers::checkAttribute(node, "classid", "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000") &&
			paramTagContainsMovieOrSrcAttribute;
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* childNode = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", ""));
		if (childNode)
		{
			GumboAttribute* movie = gumbo_get_attribute(&childNode->v.element.attributes, "movie");
			return QUrl(movie->value);
		}

		childNode = GumboParsingHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", ""));

		GumboAttribute* src = gumbo_get_attribute(&childNode->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		if (isResourceExists(url))
		{
			continue;
		}

		RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::UnknownParameter }
		};

		page->allResourcesOnPage.push_back(flashResource);
	}
}

bool FlashResourcesParser::isResourceExists(const QUrl& url) const noexcept
{
	return m_resourcesCache->isResourceExists(url);
}

}

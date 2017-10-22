#include "flash_resources_parser.h"
#include "page_parser_helpers.h"
#include "data_resources_parser.h"

namespace WebCrawler
{

FlashResourcesParser::FlashResourcesParser()
{
	addParser(std::make_shared<DataResourcesParser>(ResourceType::ResourceFlash));
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

	CompoundParser::parse(output, page);
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
			PageParserHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = PageParserHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource},
		};

		page->allResourcesOnPage.insert(flashResource);
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
			PageParserHelpers::checkAttribute(node, "type", "application/x-shockwave-flash");
	};

	auto res = [](const GumboNode* node)
	{
		GumboAttribute* src = gumbo_get_attribute(&node->v.element.attributes, "data");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = PageParserHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.insert(flashResource);
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
			PageParserHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", "")) ||
			PageParserHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", ""));

		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_OBJECT &&
			PageParserHelpers::checkAttribute(node, "classid", "clsid:D27CDB6E-AE6D-11cf-96B8-444553540000") &&
			paramTagContainsMovieOrSrcAttribute;
	};

	auto res = [](const GumboNode* node)
	{
		const GumboNode* childNode = PageParserHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("movie", ""));
		if (childNode)
		{
			GumboAttribute* movie = gumbo_get_attribute(&childNode->v.element.attributes, "movie");
			return QUrl(movie->value);
		}

		childNode = PageParserHelpers::findChildNode(node, GUMBO_TAG_PARAM, std::make_pair("src", ""));

		GumboAttribute* src = gumbo_get_attribute(&childNode->v.element.attributes, "src");
		return QUrl(src->value);
	};

	std::vector<QUrl> urls = PageParserHelpers::findNodesAndGetResult(output->root, cond, res);
	std::vector<QUrl> resolvedUrls = PageParserHelpers::resolveUrlList(page->url, urls);

	for (const QUrl& url : resolvedUrls)
	{
		const bool dataResource = url.toDisplayString().startsWith(QString("data:"));

		const RawResourceOnPage flashResource
		{
			ResourceType::ResourceFlash,
			LinkInfo{ url, LinkParameter::DofollowParameter, QString(), dataResource }
		};

		page->allResourcesOnPage.insert(flashResource);
	}
}

}

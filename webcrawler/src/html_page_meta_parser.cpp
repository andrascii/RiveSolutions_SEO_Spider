#include "html_page_meta_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageMetaParser::parse(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseMetaContentType(output, pageRaw);
	parseMetaRefresh(output, pageRaw);
	parseMetaDescription(output, pageRaw);
	parseMetaKeywords(output, pageRaw);
	parseMetaRobots(output, pageRaw);
}

void HtmlPageMetaParser::parseMetaContentType(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_META &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD &&
			GumboParsingHelpers::checkAttribute(node, "http-equiv", "content-type") &&
			GumboParsingHelpers::checkAttribute(node, "content", "");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).toLower().trimmed();
	};

	std::vector<QString> contentTypes = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!contentTypes.empty())
	{
		pageRaw->contentType = contentTypes.front();
	}
}

void HtmlPageMetaParser::parseMetaRefresh(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_META &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD &&
			GumboParsingHelpers::checkAttribute(node, "http-equiv", "refresh") &&
			GumboParsingHelpers::checkAttribute(node, "content", "");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).toLower().trimmed();
	};

	std::vector<QString> refreshes = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!refreshes.empty())
	{
		pageRaw->metaRefresh = refreshes.front();
	}
}

void HtmlPageMetaParser::parseMetaDescription(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_META &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD &&
			GumboParsingHelpers::checkAttribute(node, "name", "description") &&
			GumboParsingHelpers::checkAttribute(node, "content", "");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).toLower().trimmed();
	};

	std::vector<QString> descriptions = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!descriptions.empty())
	{
		pageRaw->metaDescription = descriptions.front();
	}
	pageRaw->hasSeveralMetaDescriptionTags = descriptions.size() > 1;
}

void HtmlPageMetaParser::parseMetaKeywords(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_META &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD &&
			GumboParsingHelpers::checkAttribute(node, "name", "keywords") &&
			GumboParsingHelpers::checkAttribute(node, "content", "");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).toLower().trimmed();
	};

	std::vector<QString> keywords = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!keywords.empty())
	{
		pageRaw->metaKeywords = keywords.front();
	}
	pageRaw->hasSeveralMetaKeywordsTags = keywords.size() > 1;
}

void HtmlPageMetaParser::parseMetaRobots(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_META &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD &&
			GumboParsingHelpers::checkAttribute(node, "name", "robots") &&
			GumboParsingHelpers::checkAttribute(node, "content", "");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).toLower().trimmed();
	};

	std::vector<QString> robots = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!robots.empty())
	{
		pageRaw->metaRobots = robots.front();
	}
}

}
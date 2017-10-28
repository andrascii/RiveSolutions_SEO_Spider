#include "meta_parser.h"
#include "page_parser_helpers.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void MetaParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseMetaContentType(output, page);
	parseMetaRefresh(output, page);
	parseMetaDescription(output, page);
	parseMetaKeywords(output, page);
	parseMetaRobots(output, headers, page);
}

void MetaParser::parseMetaContentType(GumboOutput* output, ParsedPagePtr& page) noexcept
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
		return QString(attr->value).trimmed();
	};

	std::vector<QString> contentTypes = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!contentTypes.empty())
	{
		page->contentType = contentTypes.front();
	}
}

void MetaParser::parseMetaRefresh(GumboOutput* output, ParsedPagePtr& page) noexcept
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
		return QString(attr->value).trimmed();
	};

	std::vector<QString> refreshes = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!refreshes.empty())
	{
		page->metaRefresh = refreshes.front();
	}
}

void MetaParser::parseMetaDescription(GumboOutput* output, ParsedPagePtr& page) noexcept
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
		return QString(attr->value).trimmed();
	};

	std::vector<QString> descriptions = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!descriptions.empty())
	{
		page->metaDescription = descriptions.front();
	}
	page->hasSeveralMetaDescriptionTags = descriptions.size() > 1;
}

void MetaParser::parseMetaKeywords(GumboOutput* output, ParsedPagePtr& page) noexcept
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
		return QString(attr->value).trimmed();
	};

	std::vector<QString> keywords = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);

	if (!keywords.empty())
	{
		page->metaKeywords = keywords.front();
	}

	page->hasSeveralMetaKeywordsTags = keywords.size() > 1;
}

void MetaParser::parseMetaRobots(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) noexcept
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
		return QString(attr->value).trimmed();
	};

	std::vector<QString> robots = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);

	const std::map<QString, MetaRobotsItem> metaRobotsMapping
	{
		{ QString("all"), MetaRobotsAll },
		{ QString("noindex"), MetaRobotsNoIndex },
		{ QString("nofollow"), MetaRobotsNoFollow },
		{ QString("none"), MetaRobotsNone },
		{ QString("noarchive"), MetaRobotsNoArchive },
		{ QString("nosnippet"), MetaRobotsNoSnippet },
		{ QString("noodp"), MetaRobotsNoODP },
		{ QString("notranslate"), MetaRobotsNoTranslate },
		{ QString("noimageindex"), MetaRobotsNoImageIndex },
		{ QString("index"), MetaRobotsIndex },
		{ QString("follow"), MetaRobotsFollow },
		{ QString("noyaca"), MetaRobotsNoYaCa },
		{ QString("noydir"), MetaRobotsNoYDir },
	};

	for (const QString& robotsItem : robots)
	{
		const QStringList parts = robotsItem.split(QLatin1Char(','), QString::SkipEmptyParts);
		for (const QString& part : parts)
		{
			auto it = metaRobotsMapping.find(part.trimmed().toLower());
			if (it != metaRobotsMapping.cend())
			{
				page->metaRobotsFlags.setFlag(it->second);
			}
		}
	}

	for (auto it = std::cbegin(headers); it != std::cend(headers); ++it)
	{
		if (it->first.toLower() != QString("x-robots-tag"))
		{
			continue;
		}
		const QStringList parts = QString(it->second).split(QLatin1Char(','), QString::SkipEmptyParts);
		for (const QString& part : parts)
		{
			auto it = metaRobotsMapping.find(part.trimmed().toLower());
			if (it != metaRobotsMapping.cend())
			{
				page->metaRobotsFlags.setFlag(it->second);
			}
		}
	}
}

}
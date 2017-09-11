#include "html_page_meta_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageMetaParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	parseMetaContentType(output, pageRaw);
	parseMetaRefresh(output, pageRaw);
	parseMetaDescription(output, pageRaw);
	parseMetaKeywords(output, pageRaw);
	parseMetaRobots(output, pageRaw);

	/*

	GumboNode* head = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_HEAD);

	DEBUG_ASSERT(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	std::vector<GumboNode*> metaTags = GumboParsingHelpers::subNodes(head, GUMBO_TAG_META);

	for (unsigned i = 0; i < metaTags.size(); ++i)
	{
		GumboAttribute* contentAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "content");

		if (!contentAttribute)
		{
			continue;
		}

		QString contentAttributeValue = 
			QString(contentAttribute->value).trimmed().remove(QChar('\n'), Qt::CaseInsensitive);

		GumboAttribute* metaHttpEquivAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "http-equiv");
		GumboAttribute* metaNameAttribute = gumbo_get_attribute(&metaTags[i]->v.element.attributes, "name");

		DEBUG_ASSERT(metaHttpEquivAttribute && !metaNameAttribute ||
			!metaHttpEquivAttribute && metaNameAttribute ||
			!metaHttpEquivAttribute && !metaNameAttribute);

		if (metaHttpEquivAttribute)
		{
			QString attributeValue = QString(metaHttpEquivAttribute->value).toLower().trimmed();

			if (attributeValue == "content-type")
			{
				pageRaw->contentType = contentAttributeValue;
			}

			if (attributeValue == "refresh")
			{
				pageRaw->metaRefresh = contentAttributeValue;
			}
		}

		if (metaNameAttribute)
		{
			QString attributeValue = QString(metaNameAttribute->value).toLower();

			if (attributeValue == "description")
			{
				pageRaw->metaDescription = contentAttributeValue;
			}

			if (attributeValue == "keywords")
			{
				pageRaw->metaKeywords = contentAttributeValue;
			}

			if (attributeValue == "robots")
			{
				pageRaw->metaRobots = contentAttributeValue;
			}
		}
	}
	*/
}

void HtmlPageMetaParser::parseMetaContentType(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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

void HtmlPageMetaParser::parseMetaRefresh(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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

void HtmlPageMetaParser::parseMetaDescription(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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

void HtmlPageMetaParser::parseMetaKeywords(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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

void HtmlPageMetaParser::parseMetaRobots(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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
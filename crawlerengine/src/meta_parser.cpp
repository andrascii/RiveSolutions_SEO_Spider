#include "meta_parser.h"
#include "page_parser_helpers.h"
#include "gumbo_parsing_helpers.h"
#include "meta_robots_helpers.h"

namespace CrawlerEngine
{

MetaParser::MetaParser()
{
	m_metaRefreshContentPattern = QRegularExpression("[0-9]+;[[:space:]]*url[[:space:]]*=[[:space:]]*([a-z0-9.\\-:/]+)");
}

void MetaParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		parseMetaRobots(output, headers, page);
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
			GumboParsingHelpers::checkAttribute(node, "http-equiv", "refresh");
	};

	auto res = [](const GumboNode* node)
	{
		const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
		return QString(attr->value).trimmed();
	};

	const std::vector<QString> contents = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);

	if (!contents.empty())
	{
		page->metaRefresh = contents.back();
		page->hasMetaRefreshTag = true;
	}

	foreach (const QString& content, contents)
	{
		const QRegularExpressionMatch match = m_metaRefreshContentPattern.match(content);
		const QStringList matchParts = match.capturedTexts();

		if (matchParts.size() < 2)
		{
			continue;
		}

		Url contentUrl(matchParts[1]);

		if (contentUrl == page->url)
		{
			continue;
		}

		LinkInfo link{ std::move(contentUrl), LinkParameter::DofollowParameter, QString(), false, ResourceSource::SourceTagMetaRefresh };
		page->allResourcesOnPage.emplace_back(ResourceOnPage(ResourceType::ResourceHtml, std::move(link)));
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

	const std::vector<QString> descriptions = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);

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


	std::vector<QString> xRobotsTagValues = headers.valueOf("x-robots-tag");

	for (size_t i = 0; i < xRobotsTagValues.size(); ++i)
	{
		const QStringList parts = xRobotsTagValues[i].split(QLatin1Char(','), QString::SkipEmptyParts);

		for (const QString& part : parts)
		{
			auto it = metaRobotsMapping.find(part.trimmed().toLower());

			if (it != metaRobotsMapping.cend())
			{
				page->metaRobotsFlags[UserAgentType::AnyBot].setFlag(it->second);
			}
		}
	}

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		// non-html, process only headers
		return;
	}

	const QStringList supportedUserAgents = MetaRobotsHelpers::supportedUserAgents(true);

	foreach(const QString& userAgentStr, supportedUserAgents)
	{
		const auto cond = [nameValue = userAgentStr](const GumboNode* node)
		{
			return node &&
				node->type == GUMBO_NODE_ELEMENT &&
				node->v.element.tag == GUMBO_TAG_META &&
				node->parent &&
				node->parent->v.element.tag == GUMBO_TAG_HEAD &&
				GumboParsingHelpers::checkAttribute(node, "name", nameValue.toLatin1().constData()) &&
				GumboParsingHelpers::checkAttribute(node, "content", "");
		};

		const auto res = [](const GumboNode* node)
		{
			const GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "content");
			return QString(attr->value).trimmed();
		};

		const std::vector<QString> robots = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
		const UserAgentType userAgentType = MetaRobotsHelpers::userAgent(userAgentStr);

		ASSERT(userAgentType != UserAgentType::Unknown);

		for (const QString& robotsItem : robots)
		{
			const QStringList parts = robotsItem.split(QLatin1Char(','), QString::SkipEmptyParts);

			for (const QString& part : parts)
			{
				const auto it = metaRobotsMapping.find(part.trimmed().toLower());

				if (it != metaRobotsMapping.cend())
				{
					page->metaRobotsFlags[userAgentType].setFlag(it->second);
				}
			}
		}

		
	}
}

}

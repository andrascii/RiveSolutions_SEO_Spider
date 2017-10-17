#include "h_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseH1(output, page);
	parseH2(output, page);
}

void HParser::parseH1(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_H1;
	};

	auto res = [](const GumboNode* node)
	{
		QString val = GumboParsingHelpers::nodeText(node);
		val = val.trimmed().remove(QChar('\n'), Qt::CaseInsensitive);
		return val;
	};

	std::vector<QString> h1 = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!h1.empty())
	{
		page->firstH1 = h1.front();
	}
	if (h1.size() > 1)
	{
		page->secondH1 = h1[1];
		page->hasSeveralH1Tags = true;
	}
}

void HParser::parseH2(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_H2;
	};

	auto res = [](const GumboNode* node)
	{
		QString val = GumboParsingHelpers::nodeText(node);
		val = val.trimmed().remove(QChar('\n'), Qt::CaseInsensitive);
		return val;
	};

	std::vector<QString> h2 = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!h2.empty())
	{
		page->firstH2 = h2.front();
	}
	if (h2.size() > 1)
	{
		page->secondH2 = h2[1];

		QSet<QString> set;
		for (const QString& item : h2)
		{
			set.insert(item);
		}

		page->hasSeveralEqualH2Tags = set.size() != static_cast<int>(h2.size());
	}
}

}
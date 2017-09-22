#include "html_page_h_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageHParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	parseH1(output, pageRaw);
	parseH2(output, pageRaw);
}

void HtmlPageHParser::parseH1(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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
		pageRaw->firstH1 = h1.front();
	}
	if (h1.size() > 1)
	{
		pageRaw->secondH1 = h1[1];
		pageRaw->hasSeveralH1Tags = true;
	}
}

void HtmlPageHParser::parseH2(GumboOutput* output, PageRawPtr& pageRaw) noexcept
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
		pageRaw->firstH2 = h2.front();
	}
	if (h2.size() > 1)
	{
		pageRaw->secondH2 = h2[1];
		pageRaw->hasSeveralH2Tags = true;
	}
}

}
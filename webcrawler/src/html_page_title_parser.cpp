#include "html_page_title_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageTitleParser::parse(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept
{	
	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto cond = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_TITLE &&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD;
	};

	auto res = [](const GumboNode* node)
	{
		QByteArray titleValue = GumboParsingHelpers::nodeText(node);
		titleValue = titleValue.trimmed().remove('\n', Qt::CaseInsensitive);
		return titleValue;
	};

	std::vector<QByteArray> titles = GumboParsingHelpers::findNodesAndGetResult(output->root, cond, res);
	if (!titles.empty())
	{
		pageRaw->title = titles.front();
	}
	pageRaw->hasSeveralTitleTags = titles.size() > 1;
}

}
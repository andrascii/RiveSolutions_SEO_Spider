#include "title_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

void TitleParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) noexcept
{
	Q_UNUSED(headers);

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	auto predicate = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_TITLE /*&&
			node->parent &&
			node->parent->v.element.tag == GUMBO_TAG_HEAD*/;
		// TODO: uncomment when this error will be fixed in Gumbo
	};

	auto resultGetter = [](const GumboNode* node)
	{
		QString titleValue = GumboParsingHelpers::nodeText(node);
		titleValue = titleValue.trimmed().remove("\n");
		return titleValue;
	};

	std::vector<QString> titles = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);

	if (!titles.empty())
	{
		page->title = titles.front();
	}

	page->hasSeveralTitleTags = titles.size() > 1;
}

}

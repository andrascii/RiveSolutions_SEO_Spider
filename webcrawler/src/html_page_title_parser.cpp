#include "html_page_title_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageTitleParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	GumboNode* head = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_HEAD);
	DEBUG_ASSERT(head->type == GUMBO_NODE_ELEMENT && head->v.element.tag == GUMBO_TAG_HEAD);

	GumboNode* title = GumboParsingHelpers::findSubNode(head, GUMBO_TAG_TITLE);

	if (title && title->v.element.children.length > 0)
	{
		QByteArray titleValue = GumboParsingHelpers::nodeText(static_cast<GumboNode*>(title->v.element.children.data[0]));
		titleValue = titleValue.trimmed().remove('\n', Qt::CaseInsensitive);

		pageRaw->title = titleValue;
	}
}

}
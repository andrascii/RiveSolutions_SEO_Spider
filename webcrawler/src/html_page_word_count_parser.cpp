#include "html_page_word_count_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void HtmlPageWordCountParser::parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept
{
	if (pageRaw->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

	GumboNode* body = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_BODY);
	
	if (!(body && body->type == GUMBO_NODE_ELEMENT && body->v.element.tag == GUMBO_TAG_BODY))
	{
		WARNINGLOG << "Cannot find body tag on passed page";
		return;
	}

	QByteArray textFromPage = GumboParsingHelpers::cutAllTagsFromNode(body);
	QList<QByteArray> allWordsFromPage = textFromPage.split(' ');

	pageRaw->wordCount = allWordsFromPage.size();
}

}
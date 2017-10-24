#include "word_count_parser.h"
#include "gumbo_parsing_helpers.h"

namespace WebCrawler
{

void WordCountParser::parse(GumboOutput* output, ParsedPagePtr& page) noexcept
{
	if (page->resourceType != ResourceType::ResourceHtml)
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

	page->wordCount = allWordsFromPage.size();
}

}

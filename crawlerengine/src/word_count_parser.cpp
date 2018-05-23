#include "word_count_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{


WordCountParser::WordCountParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
{
}

void WordCountParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page) noexcept
{
	Q_UNUSED(headers);

	if (page->resourceType != ResourceType::ResourceHtml)
	{
		return;
	}

// 	GumboNode* body = GumboParsingHelpers::findSubNode(output->root, GUMBO_TAG_BODY);
// 	
// 	if (!(body && body->type == GUMBO_NODE_ELEMENT && body->v.element.tag == GUMBO_TAG_BODY))
// 	{
// 		WARNLOG << "Cannot find body tag on passed page";
// 		return;
// 	}
// 
// 	QByteArray textFromPage = GumboParsingHelpers::cutAllTagsFromNode(body);
// 	QList<QByteArray> allWordsFromPage = textFromPage.split(' ');
// 
// 	page->wordCount = allWordsFromPage.size();

	//////////////////////////////////////////////////////////////////////////

	IHtmlNodeSharedPtr bodyTag = m_htmlParser->firstMatchNode(IHtmlNode::TagIdBody);

	if (!bodyTag)
	{
		WARNLOG << "Cannot find body tag on passed page";
		return;
	}

	const QByteArray textFromPage = bodyTag->cutSubNodesAndGetPlainText();
	const QList<QByteArray> allWordsFromPage = textFromPage.split(' ');

	page->wordCount = allWordsFromPage.size();
}

}

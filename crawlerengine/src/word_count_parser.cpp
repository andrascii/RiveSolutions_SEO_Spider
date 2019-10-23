#include "stdafx.h"
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

	if (page->resourceType != ResourceType::ResourceHtml || m_htmlParser->isEmpty())
	{
		return;
	}

	IHtmlNodeCountedPtr bodyTag = m_htmlParser->firstMatchNode(IHtmlNode::TagIdBody);

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

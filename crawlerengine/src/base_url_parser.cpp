#include "base_url_parser.h"
#include "ihtml_parser.h"

namespace CrawlerEngine
{

BaseUrlParser::BaseUrlParser(IHtmlParser* htmlParser)
	: m_htmlParser(htmlParser)
	, m_regExp("[\\n\\t]+")
{
}

void BaseUrlParser::parse(const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

	IHtmlNodeSharedPtr headTagNode = m_htmlParser->firstMatchNode(IHtmlNode::TagIdHead);

	if (!headTagNode)
	{
		page->baseUrl = page->url;
		return;
	}

	IHtmlNodeSharedPtr baseTagNode = headTagNode->firstMatchSubNode(IHtmlNode::TagIdBase);

	if (baseTagNode && baseTagNode->hasAttribute("href"))
	{
		const Url url = baseTagNode->attribute("href").remove(m_regExp);

		if (url.isValid())
		{
			page->baseUrl = url;
		}
		else
		{
			WARNLOG << "On page:" << page->url.toDisplayString() << "found base tag but that tag has an invalid url address";

			page->baseUrl = page->url;
		}
	}
	else
	{
		page->baseUrl = page->url;
	}
}

}
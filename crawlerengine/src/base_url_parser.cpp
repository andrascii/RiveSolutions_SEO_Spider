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

// 	auto predicate = [](const GumboNode* node)
// 	{
// 		return node &&
// 			node->type == GUMBO_NODE_ELEMENT &&
// 			node->v.element.tag == GUMBO_TAG_BASE &&
// 			gumbo_get_attribute(&node->v.element.attributes, "href");
// 	};
// 
// 	auto resultGetter = [](const GumboNode* node)
// 	{
// 		const QRegularExpression regExp("[\\n\\t]+");
// 		const GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
// 		const Url url = QString(href->value).remove(regExp);
// 		return url;
// 	};

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

// 	const std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);
// 	const auto urlCount = urls.size();
// 	
// 	DEBUG_ASSERT(urls.empty() || urlCount == 1);
// 
// 	const bool hasOneValidUrl = urlCount == 1 && urls[0].isValid();
// 
// 	if (hasOneValidUrl && !urls[0].isRelative())
// 	{
// 		page->baseUrl = urls[0];
// 	}
// 	else
// 	{
// 		if (hasOneValidUrl)
// 		{
// 			ERRLOG << "Found relative base address in <base> tag on page:" << page->url.urlStr();
// 		}
// 
// 		page->baseUrl = page->url;
// 	}
}

}
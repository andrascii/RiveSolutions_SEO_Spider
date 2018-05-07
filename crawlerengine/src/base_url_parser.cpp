#include "base_url_parser.h"
#include "gumbo_parsing_helpers.h"

namespace CrawlerEngine
{

void BaseUrlParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	Q_UNUSED(headers);

	auto predicate = [](const GumboNode* node)
	{
		return node &&
			node->type == GUMBO_NODE_ELEMENT &&
			node->v.element.tag == GUMBO_TAG_BASE &&
			gumbo_get_attribute(&node->v.element.attributes, "href");
	};

	auto resultGetter = [](const GumboNode* node)
	{
		const QRegularExpression regExp("[\\n\\t]+");
		const GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
		const Url url = QString(href->value).remove(regExp);
		return url;
	};

	const std::vector<Url> urls = GumboParsingHelpers::findNodesAndGetResult(output->root, predicate, resultGetter);
	const auto urlCount = urls.size();

	DEBUG_ASSERT(urls.empty() || urlCount == 1);

	const bool hasOneValidUrl = urlCount == 1 && urls[0].isValid();

	if (hasOneValidUrl && !urls[0].isRelative())
	{
		page->baseUrl = urls[0];
	}
	else
	{
		if (hasOneValidUrl)
		{
			ERRLOG << "Found relative base address in <base> tag on page:" << page->url.urlStr();
		}

		page->baseUrl = page->url;
	}
}

}
#include "html_page_parser.h"
#include "gumbo_parsing_helpers.h"
#include "logger_connection_service_api.h"

namespace WebCrawler
{


void HtmlPageParser::addPageRawParser(std::shared_ptr<IPageParser> pageRawParser) noexcept
{
	m_parserPack.push_back(std::move(pageRawParser));
}

void HtmlPageParser::parsePage(const QByteArray& htmlPage, PageRawPtr& pageRaw) noexcept
{
	//
	// TODO: optimize this code because for now we need double parsing of the same page
	//

	QByteArray decodedHtmlPage = GumboParsingHelpers::decodeHtmlPage(htmlPage);
	GumboOutputCreatorDestroyerGuard gumboOutput(&kGumboDefaultOptions, decodedHtmlPage);

	for (const std::shared_ptr<IPageParser>& parser : m_parserPack)
	{
		parser->parse(gumboOutput.output(), pageRaw);
	}

	m_pageUrlList.clear();
	parsePageUrlList(gumboOutput.output()->root);
}

const std::vector<QUrl>& HtmlPageParser::pageUrlList() const noexcept
{
	return m_pageUrlList;
}

void HtmlPageParser::parsePageUrlList(const GumboNode* node) noexcept
{
	if (!node || (node && node->type != GUMBO_NODE_ELEMENT))
	{
		return;
	}

	GumboAttribute* href = nullptr;

	if (node->v.element.tag == GUMBO_TAG_A && (href = gumbo_get_attribute(&node->v.element.attributes, "href")))
	{
		m_pageUrlList.push_back(QUrl(href->value));
	}

	const GumboVector* children = &node->v.element.children;

	for (unsigned int i = 0; i < children->length; ++i)
	{
		const GumboNode* child = static_cast<const GumboNode*>(children->data[i]);
		parsePageUrlList(child);
	}
}

}
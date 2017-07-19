#include "html_page_parser.h"
#include "gumbo_parsing_helpers.h"

namespace QuickieWebBot
{

void HtmlPageParser::addPageInfoParser(std::shared_ptr<IPageParser> pageInfoParser) noexcept
{
	m_parserPack.push_back(std::move(pageInfoParser));
}

void HtmlPageParser::parsePage(const QString& htmlPage, PageInfoPtr& pageInfo) noexcept
{
	GumboOutput* output = gumbo_parse_with_options(&kGumboDefaultOptions, htmlPage.toStdString().c_str(), htmlPage.size());

	for (const std::shared_ptr<IPageParser>& parser : m_parserPack)
	{
		parser->parse(output, pageInfo);
	}

	m_pageUrlList.clear();

	parsePageUrlList(output->root);

	gumbo_destroy_output(&kGumboDefaultOptions, output);
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
		parsePageUrlList(static_cast<const GumboNode*>(children->data[i]));
	}
}

}
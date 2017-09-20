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
	std::vector<Link> links = GumboParsingHelpers::parsePageUrlList(gumboOutput.output()->root);
	for (const Link& link : links)
	{
		m_pageUrlList.push_back(link.url);
	}
}

const std::vector<QUrl>& HtmlPageParser::pageUrlList() const noexcept
{
	return m_pageUrlList;
}

}
#include "compound_parser.h"

namespace WebCrawler
{

void CompoundParser::parse(GumboOutput* output, ParsedPagePtr& page)
{
	for (const std::shared_ptr<IPageParser>& parser : m_parsers)
	{
		parser->parse(output, page);
	}
}

void CompoundParser::addParser(std::shared_ptr<IPageParser> parser)
{
	m_parsers.push_back(std::move(parser));
}

void CompoundParser::clear()
{
	m_parsers.clear();
}

}
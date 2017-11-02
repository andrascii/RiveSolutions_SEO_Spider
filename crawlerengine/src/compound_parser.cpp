#include "compound_parser.h"

namespace CrawlerEngine
{

void CompoundParser::parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page)
{
	for (const std::shared_ptr<IPageParser>& parser : m_parsers)
	{
		parser->parse(output, headers, page);
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
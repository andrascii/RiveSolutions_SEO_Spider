#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class CompoundParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void addParser(std::shared_ptr<IPageParser> parser);
	virtual void clear();

private:
	std::vector<std::shared_ptr<IPageParser>> m_parsers;
};

}

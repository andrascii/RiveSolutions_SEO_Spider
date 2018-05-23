#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class CompoundParser : public IPageParser
{
public:
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;
	void addParser(std::shared_ptr<IPageParser> parser);
	void clear();

private:
	std::vector<std::shared_ptr<IPageParser>> m_parsers;
};

}

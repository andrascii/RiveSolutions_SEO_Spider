#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class HtmlResourcesParser : public CompoundParser
{
public:
	HtmlResourcesParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
};

}

#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class HtmlResourcesParser : public CompoundParser
{
public:
	HtmlResourcesParser();

	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
};

}

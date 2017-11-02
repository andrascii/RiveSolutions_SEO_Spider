#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class CssResourcesParser : public CompoundParser
{
public:
	CssResourcesParser();
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;
};

}
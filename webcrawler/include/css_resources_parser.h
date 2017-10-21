#pragma once

#include "compound_parser.h"

namespace WebCrawler
{
class ResourcesCache;

class CssResourcesParser : public CompoundParser
{
public:
	CssResourcesParser();
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
};

}
#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

	class ResourcesCache;

class JsResourcesParser : public CompoundParser
{
public:
	JsResourcesParser();
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
};

}

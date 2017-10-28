#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

	class ResourcesCache;

class JsResourcesParser : public CompoundParser
{
public:
	JsResourcesParser();
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;
};

}

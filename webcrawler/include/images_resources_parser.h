#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class ImagesResourcesParser : public CompoundParser
{
public:
	ImagesResourcesParser();
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;
};

}
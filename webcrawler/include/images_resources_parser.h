#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class ImagesResourcesParser : public CompoundParser
{
public:
	ImagesResourcesParser();
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
};

}
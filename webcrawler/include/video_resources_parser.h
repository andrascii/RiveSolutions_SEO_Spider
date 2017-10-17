#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class VideoResourcesParser : public CompoundParser
{
public:
	VideoResourcesParser(ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	ResourcesCache* m_resourcesCache;
};

}
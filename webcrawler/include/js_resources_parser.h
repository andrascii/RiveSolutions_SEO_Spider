#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

	class ResourcesCache;

class JsResourcesParser : public CompoundParser
{
public:
	JsResourcesParser(ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	ResourcesCache* m_resourcesCache;
};

}

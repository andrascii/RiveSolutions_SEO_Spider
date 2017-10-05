#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{
class ResourcesCache;

class CssResourcesParser : public IPageParser
{
public:
	CssResourcesParser(ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	ResourcesCache* m_resourcesCache;
};

}
#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class ImagesResourcesParser : public IPageParser
{
public:
	ImagesResourcesParser(ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	ResourcesCache* m_resourcesCache;
};

}
#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{
class ResourcesCache;

class DataResourcesParser : public IPageParser
{
public:
	DataResourcesParser(ResourceType resourceType, ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	ResourceType m_resourceType;
	ResourcesCache* m_resourcesCache;
};

}
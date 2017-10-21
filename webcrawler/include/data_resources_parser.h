#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{
class ResourcesCache;

class DataResourcesParser : public IPageParser
{
public:
	DataResourcesParser(ResourceType resourceType);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;

private:
	ResourceType m_resourceType;
};

}
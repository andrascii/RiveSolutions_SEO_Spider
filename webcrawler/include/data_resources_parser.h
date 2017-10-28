#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{
class ResourcesCache;

class DataResourcesParser : public IPageParser
{
public:
	DataResourcesParser(ResourceType resourceType);
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	ResourceType m_resourceType;
};

}
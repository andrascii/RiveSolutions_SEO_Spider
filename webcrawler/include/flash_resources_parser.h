#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class FlashResourcesParser : public CompoundParser
{
public:
	FlashResourcesParser(ResourcesCache* resourcesCache);
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
	virtual void init() override;

private:
	void parseFlashResourcesV1(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV2(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV3(GumboOutput* output, ParsedPagePtr& page) noexcept;

	bool isResourceExists(const QUrl& url) const noexcept;

private:
	ResourcesCache* m_resourcesCache;
};

}
#pragma once

#include "compound_parser.h"

namespace WebCrawler
{

class ResourcesCache;

class FlashResourcesParser : public CompoundParser
{
public:
	FlashResourcesParser();
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	void parseFlashResourcesV1(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV2(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV3(GumboOutput* output, ParsedPagePtr& page) noexcept;

	bool isResourceExists(const QUrl& url) const noexcept;
};

}
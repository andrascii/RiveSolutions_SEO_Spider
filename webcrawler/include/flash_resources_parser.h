#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class FlashResourcesParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;

private:
	void parseFlashResourcesV1(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV2(GumboOutput* output, ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV3(GumboOutput* output, ParsedPagePtr& page) noexcept;

	bool isResourceExists(const ParsedPagePtr& page, const QUrl& url) const noexcept;
};

}
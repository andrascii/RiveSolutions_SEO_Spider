#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class TitleParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept override;
};

}
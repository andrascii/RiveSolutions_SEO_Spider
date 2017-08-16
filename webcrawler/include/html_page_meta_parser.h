#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class HtmlPageMetaParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept override;
};

}

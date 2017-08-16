#pragma once

#include "page_raw.h"
#include "gumbo.h"

namespace WebCrawler
{

class IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept = 0;
};

}

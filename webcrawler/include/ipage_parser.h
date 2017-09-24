#pragma once

#include "parsed_page.h"
#include "gumbo.h"

namespace WebCrawler
{

class IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) = 0;
};

}

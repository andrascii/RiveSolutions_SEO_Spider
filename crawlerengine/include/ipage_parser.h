#pragma once

#include "gumbo.h"
#include "parsed_page.h"
#include "response_headers.h"

namespace CrawlerEngine
{

class IPageParser
{
public:
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) = 0;
};

}

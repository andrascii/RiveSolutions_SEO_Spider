#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class BaseUrlParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;
};

}
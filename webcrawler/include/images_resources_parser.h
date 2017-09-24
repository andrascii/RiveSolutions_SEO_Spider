#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class ImagesResourcesParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& page) override;
};

}
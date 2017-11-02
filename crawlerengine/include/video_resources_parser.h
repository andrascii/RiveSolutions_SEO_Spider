#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class VideoResourcesParser : public CompoundParser
{
public:
	VideoResourcesParser();
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page) override;
};

}
#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class WordCountParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& pageRaw) noexcept override;
};

}

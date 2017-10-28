#pragma once

#include "ipage_parser.h"
#include "gumbo.h"

namespace WebCrawler
{

class HParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& pageRaw) override;

private:
	void parseH1(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseH2(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
};

}

#pragma once

#include "ipage_parser.h"
#include "gumbo.h"

namespace WebCrawler
{

class HtmlPageHParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept override;

private:
	void parseH1(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseH2(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
};

}

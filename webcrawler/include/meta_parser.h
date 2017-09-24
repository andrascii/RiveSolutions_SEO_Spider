#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class MetaParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& pageRaw) override;

private:
	void parseMetaContentType(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaRefresh(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaDescription(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaKeywords(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseMetaRobots(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
};

}

#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class HtmlPageMetaParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept override;

private:
	void parseMetaContentType(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseMetaRefresh(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseMetaDescription(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseMetaKeywords(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseMetaRobots(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
};

}

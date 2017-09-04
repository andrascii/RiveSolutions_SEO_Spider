#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class HtmlPageResourcesParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept override;
	
	
private:
	void parseResourceType(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseHtmlResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseJavaScriptResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseStyleSheetResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
};

}
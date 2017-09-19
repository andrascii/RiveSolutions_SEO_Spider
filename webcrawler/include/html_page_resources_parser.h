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
	void parseImageResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseVideoResources(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseFlashResourcesV1(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseFlashResourcesV2(GumboOutput* output, PageRawPtr& pageRaw) noexcept;
	void parseFlashResourcesV3(GumboOutput* output, PageRawPtr& pageRaw) noexcept;

	bool resourceExists(const QUrl& resourceUrl) const noexcept;

private:
	mutable QSet<QString> m_resourcesCache;
};

}
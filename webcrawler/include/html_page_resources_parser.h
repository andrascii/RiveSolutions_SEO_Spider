#pragma once

#include "ipage_parser.h"

namespace WebCrawler
{

class HtmlPageResourcesParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept override;
	
private:
	void parseResourceType(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseHtmlResources(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseJavaScriptResources(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseStyleSheetResources(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseImageResources(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseVideoResources(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseFlashResourcesV1(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseFlashResourcesV2(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;
	void parseFlashResourcesV3(GumboOutput* output, ParsedPagePtr& pageRaw) noexcept;

	bool resourceExists(const QUrl& resourceUrl) const noexcept;

private:
	mutable QSet<QString> m_resourcesCache;
};

}
#pragma once

#include "ipage_parser.h"
#include "gumbo.h"

namespace WebCrawler
{

class HtmlPageHParser : public IPageParser
{
public:
	virtual void parse(GumboOutput* output, PageRawPtr& pageRaw) noexcept override;

private:
	void recurseSearch(const GumboNode* node, GumboTag tag, std::vector<QString>& container) noexcept;
	void recurseSearchText(const GumboNode* node, std::vector<QString>& container) noexcept;

private:
	std::vector<QString> m_h1Tags;
	std::vector<QString> m_h2Tags;
};

}

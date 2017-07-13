#pragma once

#include "gumbo.h"
#include "ipage_parser.h"

namespace QuickieWebBot
{

class HtmlPageParser
{
public:
	void addPageInfoParser(std::unique_ptr<IPageParser> webPageInfoParser) noexcept;

	void parsePage(const QString& htmlPage, PageInfoPtr& pageInfo) noexcept;

	const std::vector<QUrl>& pageUrlList() const noexcept;

private:
	void parsePageUrlList(const GumboNode* node) noexcept;

private:
	std::vector<QUrl> m_pageUrlList;
	std::vector<std::unique_ptr<IPageParser>> m_parserPack;
};

}

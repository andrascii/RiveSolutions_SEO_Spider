#pragma once

#include "ipage_parser.h"
#include "gumbo.h"

namespace WebCrawler
{

class HtmlPageParser
{
public:
	void addPageRawParser(std::shared_ptr<IPageParser> pageRawParser) noexcept;

	void parsePage(const QByteArray& htmlPage, PageRawPtr& pageRaw) noexcept;

	const std::vector<QUrl>& pageUrlList() const noexcept;

private:
	std::vector<QUrl> m_pageUrlList;
	std::vector<std::shared_ptr<IPageParser>> m_parserPack;
};

}

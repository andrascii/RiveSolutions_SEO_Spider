#pragma once

#include "gumbo.h"
#include "ipage_parser.h"

namespace QuickieWebBot
{

class HtmlPageParser
{
public:
	void addPageInfoParser(std::shared_ptr<IPageParser> pageInfoParser) noexcept;

	void parsePage(const QByteArray& htmlPage, PageInfoPtr& pageInfo) noexcept;

	const std::vector<QUrl>& pageUrlList() const noexcept;

private:
	void parsePageUrlList(const GumboNode* node) noexcept;

	QByteArray identifyHtmlPageContentType(const QByteArray& htmlPage) const noexcept;
	QByteArray decodeHtmlPage(const QByteArray& htmlPage) const noexcept;

private:
	std::vector<QUrl> m_pageUrlList;
	std::vector<std::shared_ptr<IPageParser>> m_parserPack;
};

}
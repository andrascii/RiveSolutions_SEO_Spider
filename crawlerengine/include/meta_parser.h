#pragma once
#include "stdafx.h"

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class MetaParser : public IPageParser
{
public:
	MetaParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	void parseMetaContentType(ParsedPagePtr& parsedPage) noexcept;
	void parseMetaRefresh(ParsedPagePtr& parsedPage) noexcept;
	void parseMetaDescription(ParsedPagePtr& parsedPage) noexcept;
	void parseMetaKeywords(ParsedPagePtr& parsedPage) noexcept;
	void parseMetaRobots(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) noexcept;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_metaRefreshContentPattern;
	QRegularExpression m_eraseCharsRegExp;
};

}

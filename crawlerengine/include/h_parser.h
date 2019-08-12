#pragma once
#include "stdafx.h"

#include "ipage_parser.h"
#include "gumbo.h"

namespace CrawlerEngine
{

class IHtmlParser;

class HParser : public IPageParser
{
public:
	HParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	void parseH1(ParsedPagePtr& parsedPage) noexcept;
	void parseH2(ParsedPagePtr& parsedPage) noexcept;

private:
	IHtmlParser* m_htmlParser;
};

}

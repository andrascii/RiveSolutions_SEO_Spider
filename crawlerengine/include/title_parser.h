#pragma once
#include "stdafx.h"

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class TitleParser : public IPageParser
{
public:
	TitleParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) noexcept override;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_regExp;
};

}
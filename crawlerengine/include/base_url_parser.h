#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class BaseUrlParser : public IPageParser
{
public:
	BaseUrlParser(IHtmlParser* htmlParser);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_regExp;
};

}
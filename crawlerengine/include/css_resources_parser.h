#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class CssResourcesParser : public CompoundParser
{
public:
	CssResourcesParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_regExp;
};

}
#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class YmParser : public IPageParser
{
public:
	YmParser(IHtmlParser* htmlParser);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	bool findYaCounter(const QString& javaScriptCode) const;

private:
	IHtmlParser* m_htmlParser;
};

}
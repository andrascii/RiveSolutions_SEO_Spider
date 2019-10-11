#pragma once

#include "ipage_parser.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

class IHtmlParser;

class XPathParser : public IPageParser
{
public:
	XPathParser(IHtmlParser* htmlParser, const CrawlerOptionsData& options);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
	CrawlerOptionsData m_options;
	QMap<int, QString> m_xPaths;
};

}
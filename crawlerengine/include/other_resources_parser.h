#pragma once
#include "stdafx.h"

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class OtherResourcesParser : public IPageParser
{
public:
	OtherResourcesParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
};

}

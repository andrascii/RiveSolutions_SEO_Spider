#pragma once

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class FramesDetectorParser : public IPageParser
{
public:
	FramesDetectorParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
};

}
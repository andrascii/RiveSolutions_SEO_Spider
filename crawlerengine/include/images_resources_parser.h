#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class ImagesResourcesParser : public CompoundParser
{
public:
	ImagesResourcesParser(IHtmlParser* htmlParser);

	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_regExp;
};

}
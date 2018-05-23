#pragma once

#include "compound_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class FlashResourcesParser : public CompoundParser
{
public:
	FlashResourcesParser(IHtmlParser* htmlParser);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& page) override;

private:
	void parseFlashResourcesV1(ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV2(ParsedPagePtr& page) noexcept;
	void parseFlashResourcesV3(ParsedPagePtr& page) noexcept;

private:
	IHtmlParser* m_htmlParser;
	QRegularExpression m_regExp;
};

}
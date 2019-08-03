#pragma once
#include "stdafx.h"

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class YmParser : public IPageParser
{
public:
	YmParser(IHtmlParser* htmlParser, int counterNumber, StorageType targetStorageType);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	bool findYaCounter(const QString& javaScriptCode) const;
	bool findYaCounter2(const QString& javaScriptCode) const;
	bool findYaCounter3(const QString& javaScriptCode) const;

private:
	IHtmlParser* m_htmlParser;
	int m_counterNumber;
	StorageType m_targetStorageType;
};

}
#pragma once
#include "stdafx.h"

#include "ipage_parser.h"

namespace CrawlerEngine
{

class IHtmlParser;

class GaParser : public IPageParser
{
public:
	GaParser(IHtmlParser* htmlParser, const QString& counterNumber, StorageType targetStorageType);
	virtual void parse(const ResponseHeaders& headers, ParsedPagePtr& parsedPage) override;

private:
	IHtmlParser* m_htmlParser;
	QString m_counterNumber;
	StorageType m_targetStorageType;
};

}
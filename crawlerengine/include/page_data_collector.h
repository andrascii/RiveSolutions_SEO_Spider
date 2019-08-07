#pragma once

#include "compound_parser.h"
#include "parsed_page.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

struct DownloadResponse;
class ResponseHeaders;
class Hop;
class IHtmlParser;
class HopsChain;

class PageDataCollector : public QObject
{
	Q_OBJECT

public:
	PageDataCollector(QObject* parent = nullptr);

	void setOptions(const CrawlerOptionsData& crawlerOptions) noexcept;
	std::vector<ParsedPagePtr> collectPageDataFromResponse(const HopsChain& hopsChain);

private:
	static Url resolveRedirectUrl(const Hop& hop);

	void applyOptions();
	void collectReplyData(const Hop& hop, ParsedPagePtr& page) const;
	void setResourceType(ParsedPagePtr& page) const;
	std::shared_ptr<IPageParser> createParser(ParserType parserType) const;

private:
	CompoundParser m_parser;
	std::shared_ptr<IHtmlParser> m_htmlParser;
	CrawlerOptionsData m_crawlerOptionsData;
};

}

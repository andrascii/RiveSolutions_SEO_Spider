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

class PageDataCollector : public QObject
{
	Q_OBJECT

public:
	PageDataCollector(QObject* parent = nullptr);

	void setOptions(const CrawlerOptionsData& crawlerOptions) noexcept;
	std::vector<ParsedPagePtr> collectPageDataFromResponse(const DownloadResponse& response);

private:
	static Url resolveRedirectUrl(const Hop& hop);

	void applyOptions();
	void collectReplyData(const Hop& hop, ParsedPagePtr& page) const;
	void setResourceType(ParsedPagePtr& page) const;
	std::shared_ptr<IPageParser> createParser(ParserType parserType) const;

private:
	std::shared_ptr<IHtmlParser> m_htmlParser;
	CompoundParser m_parser;
	CrawlerOptionsData m_crawlerOptionsData;
};

}

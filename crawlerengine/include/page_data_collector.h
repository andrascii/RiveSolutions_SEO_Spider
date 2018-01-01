#pragma once

#include "compound_parser.h"
#include "parsed_page.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

struct DownloadResponse;
class ResponseHeaders;
class Hop;

class PageDataCollector : public QObject
{
	Q_OBJECT

public:
	PageDataCollector(QObject* parent = nullptr);

	void setOptions(const CrawlerOptions& crawlerOptions) noexcept;
	std::vector<ParsedPagePtr> collectPageDataFromResponse(const DownloadResponse& response);
	const std::vector<LinkInfo>& outlinks() const noexcept;

private:
	static Url resolveRedirectUrl(const Hop& hop);

	void applyOptions();
	void collectReplyData(const Hop& hop, ParsedPagePtr& page) const;
	void collectParsedPageData(GumboOutput* output, const ResponseHeaders& headers, ParsedPagePtr& page);
	void setResourceCategory(ParsedPagePtr& page) const;
	std::shared_ptr<IPageParser> createParser(ParserType parserType) const;

private:
	CompoundParser m_parser;
	CrawlerOptions m_crawlerOptions;
	std::vector<LinkInfo> m_outlinks;
};

}

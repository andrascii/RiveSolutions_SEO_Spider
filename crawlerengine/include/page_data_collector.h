#pragma once

#include "queued_downloader.h"
#include "compound_parser.h"
#include "parsed_page.h"
#include "crawler_options.h"

namespace CrawlerEngine
{

struct DownloadResponse;

class PageDataCollector : public QObject
{
	Q_OBJECT

public:
	PageDataCollector(QObject* parent = nullptr);

	void setOptions(const CrawlerOptions& crawlerOptions) noexcept;

	ParsedPagePtr collectPageDataFromResponse(const DownloadResponse& response);

	const std::vector<LinkInfo>& outlinks() const noexcept;

private:
	void applyOptions();

	static QUrl resolveRedirectUrl(const DownloadResponse& response);

	void collectReplyData(const DownloadResponse& response, ParsedPagePtr& page) const;

	void collectParsedPageData(GumboOutput* output, const IPageParser::ResponseHeaders& headers, ParsedPagePtr& page);

	void collectUrlList(GumboOutput* output);

	void setResourceCategory(ParsedPagePtr& page) const;

	std::shared_ptr<IPageParser> createParser(ParserType parserType) const;

private:
	CompoundParser m_parser;

	CrawlerOptions m_crawlerOptions;

	std::vector<LinkInfo> m_outlinks;
};

}

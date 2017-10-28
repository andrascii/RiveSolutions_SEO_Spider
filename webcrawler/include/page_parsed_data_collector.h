#pragma once

#include "queued_downloader.h"
#include "compound_parser.h"
#include "parsed_page.h"
#include "crawler_options.h"

namespace WebCrawler
{

class PageParsedDataCollector : public QObject
{
	Q_OBJECT

public:
	PageParsedDataCollector(QObject* parent = nullptr);

	void setOptions(const CrawlerOptions& crawlerOptions) noexcept;

	ParsedPagePtr collectPageDataFromReply(const QueuedDownloader::Reply& reply);
	const std::vector<LinkInfo>& outlinks() const noexcept;

private:
	void applyOptions();

	static QUrl resolveRedirectUrl(const QueuedDownloader::Reply& reply);

	void collectReplyData(const QueuedDownloader::Reply& reply, ParsedPagePtr& page) const;
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

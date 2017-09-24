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

	void setOptions(const CrawlerOptions& options) noexcept;
	const CrawlerOptions& options() const noexcept;

	ParsedPagePtr collectPageDataFromReply(const QueuedDownloader::Reply& reply);
	const std::vector<QUrl>& urlList() const noexcept;

private:
	void applyOptions();

	QUrl resolveRedirectUrl(const QueuedDownloader::Reply& reply) const;

	void collectReplyData(const QueuedDownloader::Reply& reply, ParsedPagePtr& page);
	void collectParsedPageData(GumboOutput* output, ParsedPagePtr& page);
	void collectUrlList(GumboOutput* output);

private:
	CompoundParser m_parser;
	CrawlerOptions m_options;

	std::vector<QUrl> m_urlList;
};

}

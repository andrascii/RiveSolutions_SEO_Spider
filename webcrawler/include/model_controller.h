#pragma once

#include "parsed_page.h"
#include "crawler_options.h"

namespace WebCrawler
{

class UnorderedDataCollection;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController(QObject* parent, QThread* sequencedDataCollectionThread);
	~ModelController();

	void setWebCrawlerOptions(const CrawlerOptions& options);

	void addParsedPage(ParsedPagePtr parsedPagePtr) noexcept;

	const UnorderedDataCollection* data() const noexcept;
	UnorderedDataCollection* data() noexcept;

private:
	void processParsedPageUrl(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageTitle(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageMetaDescription(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageMetaKeywords(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageH1(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageH2(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageImage(ParsedPagePtr parsedPagePtr, bool checkOnlyLastResource = false) noexcept;
	void processParsedPageStatusCode(ParsedPagePtr parsedPagePtr) noexcept;

	void processParsedPageHtmlResources(ParsedPagePtr parsedPagePtr) noexcept;
	void processParsedPageResources(ParsedPagePtr parsedPagePtr) noexcept;
	void fixParsedPageResourceType(ParsedPagePtr parsedPagePtr) noexcept;

private:
	UnorderedDataCollection* m_data;
	CrawlerOptions m_crawlerOptions;
};

}
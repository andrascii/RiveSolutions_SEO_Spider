#pragma once

#include "parsed_page.h"
#include "crawler_options.h"
#include "sequenced_data_collection_types.h"

namespace CrawlerEngine
{

class UnorderedDataCollection;

class ModelController : public QObject
{
	Q_OBJECT

public:
	ModelController(QObject* parent, QThread* sequencedDataCollectionThread);

	void setWebCrawlerOptions(const CrawlerOptions& options);
	void addParsedPage(ParsedPagePtr incomingPage) noexcept;

	const UnorderedDataCollection* data() const noexcept;
	UnorderedDataCollection* data() noexcept;

private:
	void processParsedPageUrl(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageTitle(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageMetaDescription(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageMetaKeywords(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageH1(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageH2(ParsedPagePtr incomingPage) const noexcept;
	void processParsedPageImage(ParsedPagePtr incomingPage, bool checkOnlyLastResource = false) const noexcept;
	void processParsedPageStatusCode(ParsedPagePtr incomingPage) const noexcept;

	void processParsedPageHtmlResources(ParsedPagePtr incomingPage) noexcept;
	void processParsedPageResources(ParsedPagePtr incomingPage) noexcept;
	void fixParsedPageResourceType(ParsedPagePtr incomingPage) const noexcept;

	bool resourceShouldBeProcessed(ResourceType resourceType) const noexcept;

private:
	UnorderedDataCollection* m_data;
	CrawlerOptions m_crawlerOptions;

	LinksToThisResourceChanges m_linksToPageChanges;
};

}

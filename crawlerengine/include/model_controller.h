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
	ModelController();

	Q_SLOT void addParsedPage(ParsedPagePtr incomingPage) noexcept;
	Q_SLOT void setWebCrawlerOptions(const CrawlerOptions& options);
	Q_SLOT void clearData();

	const UnorderedDataCollection* data() const noexcept
	{
		return m_data;
	}

	UnorderedDataCollection * data() noexcept
	{
		return m_data;
	}

	size_t crawledStorageSize() const;
	size_t acceptedCrawledStorageSize() const;

private:
	void processParsedPageUrl(ParsedPagePtr& incomingPage);
	void processParsedPageTitle(ParsedPagePtr& incomingPage);
	void processParsedPageMetaDescription(ParsedPagePtr& incomingPage);
	void processParsedPageMetaKeywords(ParsedPagePtr& incomingPage);
	void processParsedPageH1(ParsedPagePtr& incomingPage);
	void processParsedPageH2(ParsedPagePtr& incomingPage);
	void processParsedPageImage(ParsedPagePtr& incomingPage, bool checkOnlyLastResource = false);
	void processParsedPageStatusCode(ParsedPagePtr& incomingPage);

	void processParsedPageHtmlResources(ParsedPagePtr& incomingPage);
	void processParsedPageResources(ParsedPagePtr& incomingPage);
	void fixParsedPageResourceType(ParsedPagePtr& incomingPage) const noexcept;

	bool resourceShouldBeProcessed(ResourceType resourceType) const noexcept;

	void calculatePageLevel(ParsedPagePtr& incomingPage) const noexcept;
	void setPageLevel(ParsedPagePtr& page, int level) const noexcept;

	void addDuplicates(const ParsedPagePtr& incomingPage, int lookupStorage, int destStorage);

private:
	UnorderedDataCollection* m_data;
	CrawlerOptions m_crawlerOptions;

	LinksToThisResourceChanges m_linksToPageChanges;

	mutable std::atomic<size_t> m_crawledStorageSize;
	mutable std::atomic<size_t> m_acceptedCrawledStorageSize;
};

}

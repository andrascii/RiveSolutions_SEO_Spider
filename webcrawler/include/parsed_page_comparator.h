#pragma once

#include "parsed_page.h"

namespace WebCrawler
{

struct IParsedPageComparator
{
public:
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept = 0;
};

class ParsedPageComparatorProxy
{
public:
	ParsedPageComparatorProxy(IParsedPageComparator* comparator)
		: m_comparator(comparator)
	{
	}

	bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const
	{
		return (*m_comparator)(lhs, rhs);
	}

private:
	std::shared_ptr<IParsedPageComparator> m_comparator;
};

struct ParsedPageUrlComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->url == rhs->url;
	}
};

struct ParsedPageTitleComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->title == rhs->title;
	}
};

struct ParsedPageContentTypeComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->contentType == rhs->contentType;
	}
};

struct ParsedPageMetaRefreshComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->metaRefresh == rhs->metaRefresh;
	}
};

struct ParsedPageMetaRobotsComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->metaRobots == rhs->metaRobots;
	}
};

struct ParsedPageMetaDescriptionComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->metaDescription == rhs->metaDescription;
	}
};

struct ParsedPageMetaKeywordsComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->metaKeywords == rhs->metaKeywords;
	}
};

struct ParsedPageRedirectedUrlComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->redirectedUrl == rhs->redirectedUrl;
	}
};

struct ParsedPageFirstH1Comparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->firstH1 == rhs->firstH1;
	}
};

struct ParsedPageSecondH1Comparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->secondH1 == rhs->secondH1;
	}
};

struct ParsedPageFirstH2Comparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->firstH2 == rhs->firstH2;
	}
};

struct ParsedPageSecondH2Comparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->secondH2 == rhs->secondH2;
	}
};

struct ParsedPageCanonicalLinkElementComparator : IParsedPageComparator
{
	virtual bool operator()(const ParsedPagePtr& lhs, const ParsedPagePtr& rhs) const noexcept override
	{
		return lhs->canonicalLinkElement == rhs->canonicalLinkElement;
	}
};

}

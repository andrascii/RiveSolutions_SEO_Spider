#pragma once

#include "page_raw.h"

namespace WebCrawler
{

struct IPageRawComparator
{
public:
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept = 0;
};

class PageRawComparatorProxy
{
public:
	PageRawComparatorProxy(IPageRawComparator* comparator)
		: m_comparator(comparator)
	{
	}

	bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const
	{
		return (*m_comparator)(lhs, rhs);
	}

private:
	std::shared_ptr<IPageRawComparator> m_comparator;
};

struct PageRawUrlComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->url == rhs->url;
	}
};

struct PageRawTitleComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->title == rhs->title;
	}
};

struct PageRawContentTypeComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->contentType == rhs->contentType;
	}
};

struct PageRawMetaRefreshComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->metaRefresh == rhs->metaRefresh;
	}
};

struct PageRawMetaRobotsComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->metaRobots == rhs->metaRobots;
	}
};

struct PageRawMetaDescriptionComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->metaDescription == rhs->metaDescription;
	}
};

struct PageRawMetaKeywordsComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->metaKeywords == rhs->metaKeywords;
	}
};

struct PageRawRedirectedUrlComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->redirectedUrl == rhs->redirectedUrl;
	}
};

struct PageRawFirstH1Comparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->firstH1 == rhs->firstH1;
	}
};

struct PageRawSecondH1Comparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->secondH1 == rhs->secondH1;
	}
};

struct PageRawFirstH2Comparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->firstH2 == rhs->firstH2;
	}
};

struct PageRawSecondH2Comparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->secondH2 == rhs->secondH2;
	}
};

struct PageRawCanonicalLinkElementComparator : IPageRawComparator
{
	virtual bool operator()(const PageRawPtr& lhs, const PageRawPtr& rhs) const noexcept override
	{
		return lhs->canonicalLinkElement == rhs->canonicalLinkElement;
	}
};

}

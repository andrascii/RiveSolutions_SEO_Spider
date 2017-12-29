#pragma once

#include "page_parser_helpers.h"
#include "url.h"

namespace CrawlerEngine
{

enum class DownloadRequestType
{
	RequestTypeGet,
	RequestTypeHead
};

struct CrawlerRequest
{
	Url url;
	DownloadRequestType requestType;

	bool operator==(const CrawlerRequest& other) const
	{
		return requestType == other.requestType && url.compare(other.url);
	}

	friend bool operator<(const CrawlerRequest& lhs, const CrawlerRequest& rhs)
	{
		return lhs.url < rhs.url;
	}
};

bool operator<(const CrawlerRequest& lhs, const CrawlerRequest& rhs);

}
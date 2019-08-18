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

struct CrawlerRequestHasher
{
	size_t operator()(const CrawlerRequest& item) const noexcept
	{
		return hasher(item.url.toString().toStdString()) + static_cast<size_t>(item.requestType);
	}

	boost::hash<std::string> hasher;
};

bool operator<(const CrawlerRequest& lhs, const CrawlerRequest& rhs);

}

Q_DECLARE_METATYPE(CrawlerEngine::DownloadRequestType)
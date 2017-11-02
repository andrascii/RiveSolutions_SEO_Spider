#pragma once

namespace CrawlerEngine
{

enum RequestType
{
	RequestTypeGet,
	RequestTypeHead
};

struct CrawlerRequest
{
	QUrl url;
	RequestType requestType;

	bool operator==(const CrawlerRequest& other) const
	{
		return url == other.url && requestType == other.requestType;
	}
};

}
#pragma once

namespace WebCrawler
{

enum RequestType
{
	RequestTypeGet,
	RequestTypeHead
};

struct WebCrawlerRequest
{
	QUrl url;
	RequestType requestType;

	bool operator==(const WebCrawlerRequest& other) const
	{
		return url == other.url && requestType == other.requestType;
	}
};

}
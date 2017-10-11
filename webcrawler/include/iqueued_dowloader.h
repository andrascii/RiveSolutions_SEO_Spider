#pragma once
#include "abstract_threadable_object.h"
#include "crawler_request.h"

namespace WebCrawler
{

class IQueuedDownloader
{
public:
	struct Reply
	{
		int statusCode;
		QUrl url;
		QUrl redirectUrl;
		QByteArray responseBody;
		QByteArray responseHeaderValuePairs;
		QMap<QByteArray, QByteArray> responseHeaders;
	};

	virtual ~IQueuedDownloader() = default;

	virtual void scheduleUrl(const CrawlerRequest& url) noexcept = 0;

	virtual bool extractReply(Reply& response) noexcept = 0;

	virtual void start() noexcept = 0;
	virtual void stop() noexcept = 0;

	virtual size_t unprocessedRequestCount() const noexcept = 0;
};

}
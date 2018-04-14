#pragma once

#include "iresponse.h"
#include "response_headers.h"
#include "hops_chain.h"

namespace CrawlerEngine
{

struct DownloadResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeDownload;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeDownload)

	HopsChain hopsChain;
};

struct DownloadProgressResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeDownloadProgress;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeDownloadProgress)

	quint64 bytesReceived;
	quint64 bytesTotal;
};

}
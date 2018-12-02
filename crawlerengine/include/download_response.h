#pragma once

#include "iresponse.h"
#include "hops_chain.h"
#include "crawler_request.h"

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
	DownloadRequestType requestType;
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

struct UploadProgressResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeUploadProgress;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeUploadProgress)

	quint64 bytesSent;
	quint64 bytesTotal;
};


}
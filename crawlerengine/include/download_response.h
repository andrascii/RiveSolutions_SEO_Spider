#pragma once

#include "iresponse.h"
#include "response_headers.h"

namespace CrawlerEngine
{

struct DownloadResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeDownload;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeDownload)

	int statusCode;
	Url url;
	Url redirectUrl;
	QByteArray responseBody;
	ResponseHeaders responseHeaders;
};

}
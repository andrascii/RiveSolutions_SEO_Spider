#pragma once

#include "irequest.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

struct DownloadRequest : public IRequest
{
	DownloadRequest(const CrawlerRequest& requestInfo)
		: requestInfo(requestInfo)
	{
	}

	virtual IRequest* clone() const override
	{
		return new DownloadRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestTypeDownload;
	}

	CrawlerRequest requestInfo;
};

}
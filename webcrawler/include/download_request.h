#pragma once

#include "irequest.h"
#include "crawler_request.h"

namespace WebCrawler
{

struct DownloadRequest : public Common::IRequest
{
	virtual Common::IRequest* clone() const override
	{
		return new DownloadRequest(*this);
	}
	virtual Common::RequestType requestType() const noexcept override
	{
		return Common::RequestType::RequestTypeDownload;
	}

	CrawlerRequest requestInfo;
};

}
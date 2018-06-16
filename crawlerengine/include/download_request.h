#pragma once

#include "irequest.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

struct DownloadRequest : public IRequest
{
	enum class LinkStatus
	{
		LinkStatusReloadAlreadyLoaded,
		LinkStatusFirstLoading
	};

	enum class BodyProcessingCommand
	{
		CommandDownloadBodyAnyway,
		CommandAutoDetectionBodyLoadingNecessity
	};

	DownloadRequest(const CrawlerRequest& requestInfo, 
		LinkStatus linkStatus = LinkStatus::LinkStatusFirstLoading, 
		BodyProcessingCommand bodyProcessingCommand = BodyProcessingCommand::CommandAutoDetectionBodyLoadingNecessity,
		bool useTimeout = false)
		: requestInfo(requestInfo)
		, linkStatus(linkStatus)
		, bodyProcessingCommand(bodyProcessingCommand)
		, useTimeout(useTimeout)
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
	LinkStatus linkStatus;
	BodyProcessingCommand bodyProcessingCommand;
	bool useTimeout;
};

}
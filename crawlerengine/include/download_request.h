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
		CommandAutoDetectionBodyLoading
	};

	DownloadRequest(const CrawlerRequest& requestInfo,
		LinkStatus linkStatus = LinkStatus::LinkStatusFirstLoading,
		BodyProcessingCommand bodyProcessingCommand = BodyProcessingCommand::CommandAutoDetectionBodyLoading,
		bool useTimeout = false)
		: requestInfo(requestInfo)
		, linkStatus(linkStatus)
		, bodyProcessingCommand(bodyProcessingCommand)
		, useTimeout(useTimeout)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<DownloadRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestDownload;
	}

	CrawlerRequest requestInfo;
	LinkStatus linkStatus;
	BodyProcessingCommand bodyProcessingCommand;
	bool useTimeout;
};

}
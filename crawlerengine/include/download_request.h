#pragma once

#include "irequest.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

struct DownloadRequest : public IRequest
{
	enum class Status
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
		int turnaround,
		Status linkStatus = Status::LinkStatusFirstLoading,
		BodyProcessingCommand bodyProcessingCommand = BodyProcessingCommand::CommandAutoDetectionBodyLoading,
		bool useTimeout = false,
		bool ignoreMaxParallelConnections = false)
		: requestInfo(requestInfo)
        , linkStatus(linkStatus)
		, bodyProcessingCommand(bodyProcessingCommand)
        , turnaround(turnaround)
		, useTimeout(useTimeout)
		, ignoreMaxParallelConnections(ignoreMaxParallelConnections)
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
	Status linkStatus;
	BodyProcessingCommand bodyProcessingCommand;
	int turnaround;

	//! use timeout set by settings
	bool useTimeout;

	//! this download request will be scheduled immediately ignoring the maximum connection count
	bool ignoreMaxParallelConnections;
};

}

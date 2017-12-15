#pragma once

namespace CrawlerEngine
{

enum class RequestType
{
	RequestTypeDownload,
	RequestGetHostInfo,
	RequestTypeRunTask
};

struct IRequest
{
	virtual IRequest* clone() const = 0;
	virtual RequestType requestType() const noexcept = 0;
};

}
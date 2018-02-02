#pragma once

namespace CrawlerEngine
{

enum class RequestType
{
	RequestTypeDownload,
	RequestGetHostInfo,
	RequestTypeRunTask,
	RequestCheck404IsProper
};

struct IRequest
{
	virtual IRequest* clone() const = 0;
	virtual RequestType requestType() const noexcept = 0;
};

}
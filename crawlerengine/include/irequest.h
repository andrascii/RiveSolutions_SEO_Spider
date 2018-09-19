#pragma once

namespace CrawlerEngine
{

enum class RequestType
{
	RequestTypeDownload,
	RequestGetHostInfo,
	RequestTypeRunTask,
	RequestCheck404IsProper,
	RequestSetSerialNumber,
	RequestGetSerialNumberData,
	RequestGetSerialNumberState,
	RequestTakeScreenshot
};

class IRequest
{
public:
	virtual ~IRequest() = default;
	virtual IRequest* clone() const = 0;
	virtual RequestType requestType() const noexcept = 0;
};

}
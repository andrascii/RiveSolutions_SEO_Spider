#pragma once

namespace Common
{

enum class RequestType
{
	RequestTypeDownload,
	RequestGetHostInfo
};

struct IRequest
{
	virtual IRequest* clone() const = 0;
	virtual RequestType requestType() const noexcept = 0;
};

}
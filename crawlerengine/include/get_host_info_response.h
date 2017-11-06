#pragma once

#include "iresponse.h"
#include "host_info.h"

namespace CrawlerEngine
{

struct GetHostInfoResponse : IResponse
{
	GetHostInfoResponse(HostInfo hostInfo)
		: hostInfo(hostInfo)
	{
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeGetHostInfo)

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeGetHostInfo;
	}

	HostInfo hostInfo;
};

}

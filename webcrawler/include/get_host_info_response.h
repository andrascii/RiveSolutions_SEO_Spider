#pragma once

#include "iresponse.h"
#include "host_info.h"

namespace WebCrawler
{

struct GetHostInfoResponse : Common::IResponse
{
	GetHostInfoResponse(HostInfo hostInfo)
		: hostInfo(hostInfo)
	{
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Common::ResponseType::ResponseTypeGetHostInfo)

	virtual Common::ResponseType type() const noexcept override
	{
		return Common::ResponseType::ResponseTypeGetHostInfo;
	}

	HostInfo hostInfo;
};

}

#pragma once

#include "iresponse.h"
#include "host_info.h"

namespace SeoSpider
{

struct GetHostInfoResponse : Common::IResponse
{
	GetHostInfoResponse(HostInfo hostInfo)
		: hostInfo(hostInfo)
	{
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Common::ResponseType::GetHostInfoResponseType)

	virtual Common::ResponseType type() const noexcept override
	{
		return Common::ResponseType::GetHostInfoResponseType;
	}

	HostInfo hostInfo;
};

}

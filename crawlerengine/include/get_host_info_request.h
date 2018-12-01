#pragma once

#include "irequest.h"
#include "url.h"

namespace CrawlerEngine
{

struct GetHostInfoRequest : IRequest
{
	GetHostInfoRequest(const Url& webpage)
		: webpage(webpage)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<GetHostInfoRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetHostInfo;
	}

	Url webpage;
};

}

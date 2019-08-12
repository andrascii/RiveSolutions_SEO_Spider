#pragma once
#include "stdafx.h"

#include "irequest.h"

namespace CrawlerEngine
{

class GetSerialNumberStateRequest : public IRequest
{
public:
	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<GetSerialNumberStateRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetSerialNumberState;
	}
};

}
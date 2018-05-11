#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class GetSerialNumberStateRequest : public IRequest
{
public:
	virtual IRequest* clone() const override
	{
		return new GetSerialNumberStateRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetSerialNumberState;
	}
};

}
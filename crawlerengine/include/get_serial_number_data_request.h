#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class GetSerialNumberDataRequest : public IRequest
{
public:
	virtual IRequest* clone() const override
	{
		return new GetSerialNumberDataRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetSerialNumberData;
	}
};

}
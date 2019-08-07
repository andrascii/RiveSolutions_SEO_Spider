#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class GetSerialNumberDataRequest : public IRequest
{
public:
	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<GetSerialNumberDataRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetSerialNumberData;
	}
};

}
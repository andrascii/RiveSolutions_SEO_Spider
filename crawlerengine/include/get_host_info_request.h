#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

struct GetHostInfoRequest : IRequest
{
	GetHostInfoRequest(const QByteArray& hostname)
		: hostname(hostname)
	{
	}

	virtual IRequest* clone() const override
	{
		return new GetHostInfoRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestGetHostInfo;
	}

	QByteArray hostname;
};

}

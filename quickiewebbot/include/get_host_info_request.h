#pragma once

#include "irequest.h"

namespace QuickieWebBot
{

struct GetHostInfoRequest : Common::IRequest
{
	GetHostInfoRequest(const QByteArray& hostname)
		: hostname(hostname)
	{
	}

	virtual Common::IRequest* clone() const override
	{
		return new GetHostInfoRequest(*this);
	}
	virtual Common::RequestType requestType() const noexcept override
	{
		return Common::RequestType::RequestGetHostInfo;
	}

	QByteArray hostname;
};

}

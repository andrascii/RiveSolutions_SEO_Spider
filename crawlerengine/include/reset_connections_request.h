#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class Requester;

struct ResetConnectionsRequest : public IRequest
{
	ResetConnectionsRequest(const QList<Requester*>& requestersToReset)
		: requestersToReset(requestersToReset)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<ResetConnectionsRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestResetConnections;
	}

	QList<Requester*> requestersToReset;
};

}
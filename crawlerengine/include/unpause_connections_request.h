#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class Requester;

struct UnpauseConnectionsRequest : public IRequest
{
	UnpauseConnectionsRequest(const QList<Requester*>& requestersToBeUnpaused)
		: requestersToUnpause(requestersToBeUnpaused)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<UnpauseConnectionsRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestUnpauseConnections;
	}

	QList<Requester*> requestersToUnpause;
};

}
#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

class Requester;

struct PauseConnectionsRequest : public IRequest
{
	PauseConnectionsRequest(const QList<Requester*>& requestersToBePaused)
		: requestersToBePaused(requestersToBePaused)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<PauseConnectionsRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestPauseConnections;
	}

	QList<Requester*> requestersToBePaused;
};

}
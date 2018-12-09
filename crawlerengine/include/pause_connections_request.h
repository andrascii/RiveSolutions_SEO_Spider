#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

struct PauseConnectionsRequest : public IRequest
{
	PauseConnectionsRequest(const QVector<const void*>& requestersToBePaused)
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

	QVector<const void*> requestersToBePaused;
};

}
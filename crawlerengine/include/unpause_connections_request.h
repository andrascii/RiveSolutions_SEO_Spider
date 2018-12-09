#pragma once

#include "irequest.h"

namespace CrawlerEngine
{

struct UnpauseConnectionsRequest : public IRequest
{
	UnpauseConnectionsRequest(const QVector<const void*>& requestersToBeUnpaused)
		: requestersToBeUnpaused(requestersToBeUnpaused)
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

	QVector<const void*> requestersToBeUnpaused;
};

}
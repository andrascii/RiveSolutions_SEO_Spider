#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

struct ResetConnectionsResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseResetConnections;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseResetConnections)
};

}
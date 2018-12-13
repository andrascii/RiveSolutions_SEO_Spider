#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

struct UnpauseResponse : public IResponse
{
	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseUnpauseRequesters;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseUnpauseRequesters)
};

}
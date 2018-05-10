#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

class GetSerialNumberStateResponse : public IResponse
{
public:
	GetSerialNumberStateResponse(int state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseGetSerialNumberState;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseGetSerialNumberState)

	int state() const
	{
		return m_state;
	}

private:
	int m_state;
};

}
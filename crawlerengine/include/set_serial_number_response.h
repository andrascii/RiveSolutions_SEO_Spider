#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

class SetSerialNumberResponse : public IResponse
{
public:
	SetSerialNumberResponse(int state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseSetSerialNumber;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseSetSerialNumber)

	int state() const
	{
		return m_state;
	}

private:
	int m_state;
};

}
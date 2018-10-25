#pragma once

#include "iresponse.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class SetSerialNumberResponse : public IResponse
{
public:
	SetSerialNumberResponse(const SerialNumberStates& state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseSetSerialNumber;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseSetSerialNumber)

	const SerialNumberStates& state() const
	{
		return m_state;
	}

private:
	SerialNumberStates m_state;
};

}
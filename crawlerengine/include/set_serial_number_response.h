#pragma once

#include "iresponse.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class SetSerialNumberResponse : public IResponse
{
public:
	SetSerialNumberResponse(const SerialNumberStates& state)
		: m_states(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseSetSerialNumber;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseSetSerialNumber)

	const SerialNumberStates& states() const
	{
		return m_states;
	}

private:
	SerialNumberStates m_states;
};

}
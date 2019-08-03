#pragma once
#include "stdafx.h"

#include "iresponse.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class GetSerialNumberStateResponse : public IResponse
{
public:
	GetSerialNumberStateResponse(SerialNumberStates state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseGetSerialNumberState;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseGetSerialNumberState)

	const SerialNumberStates& state() const
	{
		return m_state;
	}

private:
	SerialNumberStates m_state;
};

}
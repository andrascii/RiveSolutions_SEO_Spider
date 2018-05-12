#pragma once

#include "iresponse.h"
#include "license_state.h"

namespace CrawlerEngine
{

class GetSerialNumberStateResponse : public IResponse
{
public:
	GetSerialNumberStateResponse(QFlag state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseGetSerialNumberState;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseGetSerialNumberState)

	LicenseStateFlags state() const
	{
		return m_state;
	}

private:
	LicenseStateFlags m_state;
};

}
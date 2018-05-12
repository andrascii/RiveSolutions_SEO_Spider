#pragma once

#include "iresponse.h"
#include "license_state.h"

namespace CrawlerEngine
{

class SetSerialNumberResponse : public IResponse
{
public:
	SetSerialNumberResponse(QFlag state)
		: m_state(state)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseSetSerialNumber;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseSetSerialNumber)

	LicenseStateFlags state() const
	{
		return m_state;
	}

private:
	LicenseStateFlags m_state;
};

}
#pragma once

#include "iresponse.h"
#include "serial_number_data.h"

namespace CrawlerEngine
{

class GetSerialNumberDataResponse : public IResponse
{
public:
	GetSerialNumberDataResponse(const VMProtectSerialNumberData& data)
		: m_data(data)
	{
	}

	GetSerialNumberDataResponse(const SerialNumberData& data)
		: m_data(data)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseGetSerialNumberData;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseGetSerialNumberData)

	const SerialNumberData& data() const
	{
		return m_data;
	}

private:
	SerialNumberData m_data;
};

}
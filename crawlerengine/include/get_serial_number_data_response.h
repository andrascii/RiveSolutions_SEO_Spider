#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

class GetSerialNumberDataResponse : public IResponse
{
public:
	GetSerialNumberDataResponse(const VMProtectSerialNumberData& data)
		: m_data(data)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseGetSerialNumberData;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseGetSerialNumberData)

	const VMProtectSerialNumberData& data() const
	{
		return m_data;
	}

private:
	VMProtectSerialNumberData m_data;
};

}
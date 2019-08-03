#pragma once
#include "stdafx.h"

#include "irequest.h"

namespace CrawlerEngine
{

class SetSerialNumberRequest : public IRequest
{
public:
	SetSerialNumberRequest(const QByteArray& serialNumber)
		: m_serialNumber(serialNumber)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<SetSerialNumberRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestSetSerialNumber;
	}

	const QByteArray& serialNumber() const
	{
		return m_serialNumber;
	}

private:
	QByteArray m_serialNumber;
};

}
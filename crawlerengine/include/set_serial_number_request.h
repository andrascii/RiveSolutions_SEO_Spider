#pragma once

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

	virtual IRequest* clone() const override
	{
		return new SetSerialNumberRequest(*this);
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
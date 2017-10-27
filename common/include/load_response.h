#pragma once

#include "iresponse.h"

namespace Common
{

class LoadResponse : public IResponse
{
public:
	DECLARE_RESPONSE_STATIC_TYPE(ResponseType::LoadResponseType)

	void setBody(const QByteArray& body);
	const QByteArray& body() const;

	virtual ResponseType type() const noexcept;

private:
	QByteArray m_body;
};

}

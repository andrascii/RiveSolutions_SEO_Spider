#include "load_response.h"

namespace Common
{

DEFINE_RESPONSE_STATIC_TYPE(LoadResponse, ResponseType::LoadResponseType)

void LoadResponse::setBody(const QByteArray& body)
{
	m_body = body;
}

const QByteArray& LoadResponse::body() const
{
	return m_body;
}

ResponseType LoadResponse::type() const noexcept
{
	return ResponseType::LoadResponseType;
}

}
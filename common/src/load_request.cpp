#include "load_request.h"

namespace Common
{

LoadRequest::LoadRequest(QUrl url)
	: m_url(url)
{
}

const QUrl& LoadRequest::url() const noexcept
{
	return m_url;
}

IRequest* LoadRequest::clone() const
{
	return new LoadRequest(m_url);
}

RequestType LoadRequest::requestType() const noexcept
{
	return RequestType::RequestTypeLoad;
}

}
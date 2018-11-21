#include "curl_http_headers_wrapper.h"

namespace CrawlerEngine

{
CurlHttpHeadersWrapper::CurlHttpHeadersWrapper()
	: m_head(nullptr)
{
}

CurlHttpHeadersWrapper::~CurlHttpHeadersWrapper()
{
	curl_slist_free_all(m_head);
}

void CurlHttpHeadersWrapper::add(const std::string& name, const std::string& value)
{
	m_head = curl_slist_append(m_head, (name + ": " + value).c_str());

	if (!m_head)
	{
		throw std::runtime_error("Cannot add header");
	}
}

curl_slist* CurlHttpHeadersWrapper::get() const
{
	return m_head;
}

}

#pragma once

#include "hops_chain.h"
#include "curl_http_headers_wrapper.h"

namespace CrawlerEngine
{

class CurlHttpGetRequest
{
public:
	CurlHttpGetRequest(CURL* httpConnection, const Url& url);
	CurlHttpGetRequest(CurlHttpGetRequest&) = delete;
	CurlHttpGetRequest& operator=(CurlHttpGetRequest&) = delete;

	virtual ~CurlHttpGetRequest() = default;
	HopsChain execute();

protected:
	virtual void setCustomRequestOptions();

	template <typename T>
	void setOption(CURLoption option, const T& value) const
	{
		const auto result = curl_easy_setopt(m_httpConnection, option, value);

		if (result != CURLE_OK)
		{
			std::stringstream formatter;

			formatter << "Cannot set HTTP session option ("
				<< "option: " << option
				<< ", message: \"" << curl_easy_strerror(result) << "\"";

			throw std::runtime_error(formatter.str());
		}
	}

	void addRequestHeader(const std::string& name, const std::string& value);

private:
	Hop loadUrl(const Url& url);
	long getHttpCode() const;

	static size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata);
	static size_t writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userdata);

private:
	CURL* const m_httpConnection;
	const Url m_url;
	CurlHttpHeadersWrapper m_headers;
};

}

#pragma once

#include "hops_chain.h"
#include "curl_http_headers_wrapper.h"

namespace CrawlerEngine
{

class CurlHttpRequest
{
public:
	CurlHttpRequest(CURL* httpConnection, const Url& url);
	virtual ~CurlHttpRequest() = default;
	CurlHttpRequest(CurlHttpRequest&) = delete;
	CurlHttpRequest& operator=(CurlHttpRequest&) = delete;

	HopsChain execute();
	void setTimeout(int ms) const;
	void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) const;
	void setMaxRedirects(int redirects);
	void addRequestHeader(const QByteArray& name, const QByteArray& value);

protected:
	template <typename T>
	void setOption(CURLoption option, const T& value) const
	{
		const auto result = curl_easy_setopt(m_httpConnection, option, value);

		if (result != CURLE_OK)
		{
			ERRLOG
				<< "Cannot set HTTP session option ("
				<< "option: " << option
				<< ", message: \"" << curl_easy_strerror(result) << "\"";

			DEBUG_ASSERT(result == CURLE_OK);
		}
	}

private:
	virtual void setCustomRequestOptions();

private:
	Hop loadUrl(const Url& url);
	long getHttpCode() const;

	static size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata);
	static size_t writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userdata);

private:
	CURL* const m_httpConnection;
	const Url m_url;
	CurlHttpHeadersWrapper m_headers;
	int m_redirects;
};

}

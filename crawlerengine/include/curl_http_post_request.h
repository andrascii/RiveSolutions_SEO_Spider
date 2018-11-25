#pragma once

#include "curl_http_request.h"

namespace CrawlerEngine
{

class CurlHttpPostRequest : public CurlHttpRequest
{
public:
	CurlHttpPostRequest(CURL* httpConnection,
		const Url& url,
		const QByteArray& uploadData);

private:
	virtual void setCustomRequestOptions() override;

	static size_t readCallback(void* buffer, size_t size, size_t nmemb, void* userdata);

private:
	const QByteArray& m_uploadData;
	const char* m_unsentPtr;
	size_t m_unsentLength;
};

}

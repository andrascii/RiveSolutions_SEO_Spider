#include "curl_http_post_request.h"

namespace CrawlerEngine
{

CurlHttpPostRequest::CurlHttpPostRequest(CURL* httpConnection, const Url& url, const QByteArray& uploadData)
	: CurlHttpRequest(httpConnection, url)
	, m_uploadData(uploadData)
	, m_unsentPtr(uploadData.constData())
	, m_unsentLength(uploadData.size())
{
}

void CurlHttpPostRequest::setCustomRequestOptions()
{
	setOption(CURLOPT_POST, 1L);
	setOption(CURLOPT_POSTFIELDSIZE, m_uploadData.length());
	setOption(CURLOPT_READFUNCTION, readCallback);
	setOption(CURLOPT_READDATA, this);
}

size_t CurlHttpPostRequest::readCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	CurlHttpPostRequest* that = reinterpret_cast<CurlHttpPostRequest*>(userdata);

	const auto bufferSize = size * nmemb;
	const auto copySize = that->m_unsentLength < bufferSize ? that->m_unsentLength : bufferSize;

	std::copy(that->m_unsentPtr, that->m_unsentPtr + copySize,
		reinterpret_cast<char*>(buffer));

	that->m_unsentLength -= copySize;
	that->m_unsentPtr += copySize;

	return copySize;
}

}

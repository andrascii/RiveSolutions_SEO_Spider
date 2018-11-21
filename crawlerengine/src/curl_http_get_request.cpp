#include "curl_http_get_request.h"

namespace CrawlerEngine
{

CurlHttpGetRequest::CurlHttpGetRequest(CURL* httpConnection, const Url& url)
	: m_httpConnection(httpConnection)
	, m_url(url)
{
}

HopsChain CurlHttpGetRequest::execute()
{
	HopsChain hopsChain;

	bool continueLoading = false;
	const Url* url = &m_url;

	do
	{
		Hop hop = loadUrl(*url);

		const bool isRedirected =
			hop.statusCode() == Common::StatusCode::MovedPermanently301 ||
			hop.statusCode() == Common::StatusCode::MovedTemporarily302;

		hopsChain.addHop(std::move(hop));

		if (isRedirected)
		{
			continueLoading = true;
			url = &hopsChain.lastHop().redirectUrl();
		}
		else
		{
			continueLoading = false;
		}

	} while (continueLoading);

	return hopsChain;
}

void CurlHttpGetRequest::setCustomRequestOptions()
{
}

void CurlHttpGetRequest::addRequestHeader(const std::string& name, const std::string& value)
{
	m_headers.add(name, value);
}

Hop CurlHttpGetRequest::loadUrl(const Url& url)
{
	Hop hop;
	QByteArray responseHeadersData;

	curl_easy_reset(m_httpConnection);
	setOption(CURLOPT_URL, url.toDisplayString().toStdString().c_str());

	// TODO: avoid copy of response headers data
	setOption(CURLOPT_WRITEFUNCTION, &writeBodyCallback);
	setOption(CURLOPT_WRITEDATA, &hop.body());

	setOption(CURLOPT_HEADERFUNCTION, &writeResponseHeadersCallback);
	setOption(CURLOPT_HEADERDATA, &responseHeadersData);

	char errorMessage[CURL_ERROR_SIZE];
	setOption(CURLOPT_ERRORBUFFER, &errorMessage);
	setOption(CURLOPT_SSL_VERIFYPEER, 0);

	setCustomRequestOptions();
	setOption(CURLOPT_HTTPHEADER, m_headers.get());

	const CURLcode result = curl_easy_perform(m_httpConnection);

	if (result != CURLE_OK)
	{
		std::stringstream formatter;

		formatter << "Cannot perform HTTP request ("
			<< "result: " << result
			<< ", message: " << errorMessage << ")";

		throw std::runtime_error(formatter.str());
	}

	ResponseHeaders responseHeaders;
	responseHeaders.buildFromByteArray(responseHeadersData);

	hop.setUrl(url);
	hop.setStatusCode(static_cast<Common::StatusCode>(getHttpCode()));

	const std::vector<QString> locationHeaderData = responseHeaders.valueOf("location");

	// supposed that we've got only one location header
	// so we use locationHeaderData[0] to get the refirect url
	if (!locationHeaderData.empty())
	{
		const Url redirectUrl = locationHeaderData[0];
		hop.setRedirectUrl(redirectUrl);
	}

	hop.setResponseHeaders(std::move(responseHeaders));

	return hop;
}

long CurlHttpGetRequest::getHttpCode() const
{
	long httpCode = 0;
	const auto result = curl_easy_getinfo(m_httpConnection, CURLINFO_RESPONSE_CODE, &httpCode);

	if (result != CURLE_OK)
	{
		std::stringstream formatter;
		formatter << "Cannot get HTTP code (" << curl_easy_strerror(result) << ")";

		throw std::runtime_error(formatter.str());
	}

	return httpCode;
}

size_t CurlHttpGetRequest::writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	QByteArray* dataReceived = reinterpret_cast<QByteArray*>(userdata);
	const auto bufferSize = size * nmemb;
	dataReceived->append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

size_t CurlHttpGetRequest::writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	QByteArray* dataReceived = reinterpret_cast<QByteArray*>(userdata);
	const auto bufferSize = size * nmemb;
	dataReceived->append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

}
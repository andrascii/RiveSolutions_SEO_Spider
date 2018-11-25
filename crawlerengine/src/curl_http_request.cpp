#include "curl_http_request.h"

namespace
{

bool proxyAddressHasScheme(const QString& proxyHostName)
{
	return proxyHostName.startsWith("http://") ||
		proxyHostName.startsWith("https://") ||
		proxyHostName.startsWith("socks4://") ||
		proxyHostName.startsWith("socks4a://") ||
		proxyHostName.startsWith("socks5://") ||
		proxyHostName.startsWith("socks5h://");
}

}

namespace CrawlerEngine
{

CurlHttpRequest::CurlHttpRequest(CURL* httpConnection, const Url& url)
	: m_httpConnection(httpConnection)
	, m_url(url)
	, m_redirects(-1)
{
}

HopsChain CurlHttpRequest::execute()
{
	HopsChain hopsChain;

	const Url* url = &m_url;

	int redirectCount = 0;

	while (true)
	{
		Hop hop = loadUrl(*url);

		const bool isRedirected =
			hop.statusCode() == Common::StatusCode::MovedPermanently301 ||
			hop.statusCode() == Common::StatusCode::MovedTemporarily302;

		hopsChain.addHop(std::move(hop));

		if (isRedirected)
		{
			if (m_redirects != -1 && redirectCount >= m_redirects)
			{
				hop.setStatusCode(Common::StatusCode::TooManyRedirections);
				break;
			}

			++redirectCount;
			url = &hopsChain.lastHop().redirectUrl();
		}
		else
		{
			break;
		}
	}

	return hopsChain;
}

void CurlHttpRequest::setTimeout(int ms) const
{
	setOption(CURLOPT_TIMEOUT_MS, ms);
}

void CurlHttpRequest::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) const
{
	setOption(CURLOPT_PROXY, proxyHostName.toUtf8().constData());
	setOption(CURLOPT_PROXYPORT, proxyPort);

	// TODO: should we do it?
	if (!proxyAddressHasScheme(proxyHostName))
	{
		setOption(CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
	}

	if (!proxyUser.isEmpty() && !proxyPassword.isEmpty())
	{
		const QByteArray& userPasswordString = (proxyUser + ":" + proxyPassword).toUtf8();
		setOption(CURLOPT_PROXYUSERPWD, userPasswordString.constData());
	}
}

void CurlHttpRequest::setMaxRedirects(int redirects)
{
	m_redirects = redirects;
}

void CurlHttpRequest::setCustomRequestOptions()
{
}

void CurlHttpRequest::addRequestHeader(const QByteArray& name, const QByteArray& value)
{
	m_headers.add(name, value);
}

Hop CurlHttpRequest::loadUrl(const Url& url)
{
	Hop hop;
	QByteArray responseHeadersData;

	curl_easy_reset(m_httpConnection);

	setOption(CURLOPT_CONNECTTIMEOUT, 30);
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

	if (result != CURLE_OK &&
		result != CURLE_OPERATION_TIMEDOUT)
	{
		ERRLOG
			<< "Cannot perform HTTP request ("
			<< "result: " << result
			<< ", message: " << errorMessage << ")";
	}

	ResponseHeaders responseHeaders;
	responseHeaders.buildFromByteArray(responseHeadersData);

	hop.setUrl(url);

	if (result == CURLE_OPERATION_TIMEDOUT)
	{
		hop.setStatusCode(Common::StatusCode::Timeout);
	}
	else
	{
		hop.setStatusCode(static_cast<Common::StatusCode>(getHttpCode()));
	}

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

long CurlHttpRequest::getHttpCode() const
{
	long httpCode = 0;
	const auto result = curl_easy_getinfo(m_httpConnection, CURLINFO_RESPONSE_CODE, &httpCode);

	if (result != CURLE_OK)
	{
		ERRLOG
			<< "Cannot get HTTP code ("
			<< curl_easy_strerror(result)
			<< ")";
	}

	return httpCode;
}

size_t CurlHttpRequest::writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	QByteArray* dataReceived = reinterpret_cast<QByteArray*>(userdata);
	const auto bufferSize = size * nmemb;
	dataReceived->append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

size_t CurlHttpRequest::writeResponseHeadersCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	QByteArray* dataReceived = reinterpret_cast<QByteArray*>(userdata);
	const auto bufferSize = size * nmemb;
	dataReceived->append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

}

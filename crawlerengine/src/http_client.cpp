#include "http_client.h"
#include "curl_http_get_request.h"

namespace CrawlerEngine
{

HttpClient::HttpClient()
	: m_connection(initCurl())
{
}

HopsChain HttpClient::get(const Url& url)
{
	return CurlHttpGetRequest(m_connection, url).execute();
}

HopsChain HttpClient::post(const Url& url)
{
	return CurlHttpGetRequest(m_connection, url).execute();
}

CURL* HttpClient::initCurl()
{
	CURL* const result = curl_easy_init();

	if (!result)
	{
		throw std::runtime_error("Cannot initialize CURL");
	}

	return result;
}

}

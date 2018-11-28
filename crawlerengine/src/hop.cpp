#include "hop.h"

namespace CrawlerEngine
{

Hop::Hop(const Url& url, const Url& redirectUrl, Common::StatusCode statusCode, const QByteArray& body, const ResponseHeaders& responseHeaders, int elapsedTime)
	: m_url(url)
	, m_redirectUrl(redirectUrl)
	, m_statusCode(statusCode)
	, m_body(body)
	, m_responseHeaders(responseHeaders)
	, m_elapsedTime(elapsedTime)
{
}

const Url& Hop::url() const noexcept
{
	return m_url;
}

Url& Hop::url() noexcept
{
	return m_url;
}

void Hop::setUrl(const Url& url) noexcept
{
	m_url = url;
}

const Url& Hop::redirectUrl() const noexcept
{
	return m_redirectUrl;
}

Url& Hop::redirectUrl() noexcept
{
	return m_redirectUrl;
}

void Hop::setRedirectUrl(const Url& redirectUrl) noexcept
{
	m_redirectUrl = redirectUrl;
}

Common::StatusCode Hop::statusCode() const noexcept
{
	return m_statusCode;
}

void Hop::setStatusCode(Common::StatusCode statusCode) noexcept
{
	m_statusCode = statusCode;
}

const QByteArray& Hop::body() const noexcept
{
	return m_body;
}

QByteArray& Hop::body() noexcept
{
	return m_body;
}

void Hop::setBody(const QByteArray& body) noexcept
{
	m_body = body;
}

const ResponseHeaders& Hop::responseHeaders() const noexcept
{
	return m_responseHeaders;
}

ResponseHeaders& Hop::responseHeaders() noexcept
{
	return m_responseHeaders;
}

void Hop::setResponseHeaders(const ResponseHeaders& responseHeaders) noexcept
{
	m_responseHeaders = responseHeaders;
}

int Hop::elapsedTime() const
{
	return m_elapsedTime;
}

void Hop::setElapsedTime(int elapsedTime)
{
	m_elapsedTime = elapsedTime;
}

}

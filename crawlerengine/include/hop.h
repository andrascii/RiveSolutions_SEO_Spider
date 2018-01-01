#pragma once

#include "url.h"
#include "response_headers.h"
#include "status_code.h"

namespace CrawlerEngine
{

class Hop final
{
public:
	Hop() = default;

	Hop(
		const Url& url,
		const Url& redirectUrl,
		Common::StatusCode statusCode, 
		const QByteArray& body, 
		const ResponseHeaders& responseHeaders
	);

	Url& url() noexcept;
	const Url& url() const noexcept;
	void setUrl(const Url& url) noexcept;

	Url& redirectUrl() noexcept;
	const Url& redirectUrl() const noexcept;
	void setRedirectUrl(const Url& redirectUrl) noexcept;

	Common::StatusCode statusCode() const noexcept;
	void setStatusCode(Common::StatusCode statusCode) noexcept;

	QByteArray& body() noexcept;
	const QByteArray& body() const noexcept;
	void setBody(const QByteArray& body) noexcept;

	ResponseHeaders& responseHeaders() noexcept;
	const ResponseHeaders& responseHeaders() const noexcept;
	void setResponseHeaders(const ResponseHeaders& responseHeaders) noexcept;

private:
	Url m_url;
	Url m_redirectUrl;
	Common::StatusCode m_statusCode;
	QByteArray m_body;
	ResponseHeaders m_responseHeaders;
};

}
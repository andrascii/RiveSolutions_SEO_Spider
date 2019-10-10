#include "status_code_description.h"

namespace Common
{

static QMap<StatusCode, QString>& descriptions()
{
	static QMap<StatusCode, QString> s_statusCodeDescriptions
	{
		{ StatusCode::Continue100, "100 Continue" },
		{ StatusCode::SwitchingProtocol101, "101 Switching Protocol" },

		{ StatusCode::Ok200, "200 OK" },
		{ StatusCode::Created201, "201 Created" },
		{ StatusCode::Accepted202, "202 Accepted" },
		{ StatusCode::NonAuthoritativeInformation203, "203 Non Authoritative Information" },
		{ StatusCode::NoContent204, "204 No Content" },
		{ StatusCode::ResetContent205, "205 Reset Content" },
		{ StatusCode::PartialContent206, "206 Partial Content" },
		{ StatusCode::MultiStatus207, "207 Multi Status" },
		{ StatusCode::AlreadyReported208, "208 Already Reported" },
		{ StatusCode::ImUsed226, "226 I'm Used" },

		{ StatusCode::MultipleChoices300, "300 Multiple Choices" },
		{ StatusCode::MovedPermanently301, "301 Moved Permanently" },
		{ StatusCode::MovedTemporarily302, "302 Moved Temporarily" },
		{ StatusCode::SeeOther303, "303 See Other" },
		{ StatusCode::NotModified304, "304 Not Modified" },
		{ StatusCode::UseProxy305, "305 Use Proxy" },
		{ StatusCode::TemporaryRedirect307, "307 Temporary Redirect" },
		{ StatusCode::PermanentRedirect308, "308 Permanent Redirect" },

		{ StatusCode::BadRequest400, "400 Bad Request" },
		{ StatusCode::Unauthorized401, "401 Unauthorized" },
		{ StatusCode::PaymentRequired402, "402 Payment Required" },
		{ StatusCode::Forbidden403, "403 Forbidden" },
		{ StatusCode::NotFound404, "404 Not Found" },
		{ StatusCode::MethodNotAllowed405, "405 Method Not Allowed" },
		{ StatusCode::NonAcceptable406, "406 Non Acceptable" },
		{ StatusCode::ProxyAuthenticationRequired407, "407 Proxy Authentication Required" },
		{ StatusCode::RequestTimeout408, "408 Request Timeout" },
		{ StatusCode::Conflict409, "409 Conflict" },
		{ StatusCode::Gone410, "410 Gone" },
		{ StatusCode::LengthRequired411, "411 Length Required" },
		{ StatusCode::PreconditionFailed412, "412 Precondition Failed" },
		{ StatusCode::RequestEntityTooLarge413, "413 Request Entity Too Large" },
		{ StatusCode::RequestUriTooLong414, "414 Request Uri Too Long" },
		{ StatusCode::UnsupportedMediaType415, "415 Unsupported Media Type" },
		{ StatusCode::RangeNotSatisfiable416, "416 Range Not Satisfiable" },
		{ StatusCode::ExpectationFailed417, "417 Expectation Failed" },
		{ StatusCode::ImTeapot418, "418 I'm a Teapot" },
		{ StatusCode::AuthenticationTimeout419, "419 Authentication Timeout" },
		{ StatusCode::MisdirectedRequest421, "421 Misdirected Request" },
		{ StatusCode::UnprocessableEntity422, "422 Unprocessable Entity" },
		{ StatusCode::Locked423, "423 Locked" },
		{ StatusCode::FailedDependency424, "424 Failed Dependency" },
		{ StatusCode::UpgradeRequired426, "426 Upgrade Required" },
		{ StatusCode::PreconditionRequired428, "428 Precondition Required" },
		{ StatusCode::TooManyRequests429, "429 Too Many Requests" },
		{ StatusCode::RequestHeaderFieldsTooLarge431, "431 Request Header Fields Too Large" },
		{ StatusCode::RetryWith449, "449 Retry With" },
		{ StatusCode::UnavailableForLegalReasons451, "451 Unavailable For Legal Reasons" },
		{ StatusCode::BadSendedRequest452, "452 Bad Sended Request" },
		{ StatusCode::ClientClosedRequest499, "499 Client Closed Request" },

		{ StatusCode::InternalServerError500, "500 Internal Server Error" },
		{ StatusCode::NotImplemented501, "501 Not Implemented" },
		{ StatusCode::BadGateway502, "502 Bad Gateway" },
		{ StatusCode::ServiceUnavailable503, "503 Service Unavailable" },
		{ StatusCode::GatewayTimeout504, "504 Gateway Timeout" },
		{ StatusCode::HttpVersionNotSupported505, "505 HTTP Version Not Supported" },
		{ StatusCode::VariantAlsoNegotiates506, "506 Variant Also Negotiates" },
		{ StatusCode::InsufficientStorage507, "507 Insufficient Storage" },
		{ StatusCode::LoopDetected508, "508 Loop Detected" },
		{ StatusCode::BandwidthLimitExceeded509, "509 Bandwidth Limit Exceeded" },
		{ StatusCode::NotExtended510, "510 Not Extended" },
		{ StatusCode::NetworkAuthenticationRequired511, "511 Network Authentication Required" },
		{ StatusCode::UnknownError520, "520 Unknown Error" },
		{ StatusCode::WebServerIsDown521, "521 Web Server Is Down" },
		{ StatusCode::ConnectionTimedOut522, "522 Connection Timed Out" },
		{ StatusCode::OriginIsUnreachable523, "523 Origin Is Unreachable" },
		{ StatusCode::ATimeoutOccurred524, "524 A Timeout Occurred" },
		{ StatusCode::SslHandshakeFailed525, "525 SSL Handshake Failed" },
		{ StatusCode::InvalidSslCertificate526, "526 Invalid SSL Certificate" },

		{ StatusCode::BlockedByRobotsTxt, QObject::tr("Blocked By robots.txt") },
		{ StatusCode::BlockedByMetaRobots, QObject::tr("Blocked By Meta Robots") },
		{ StatusCode::Timeout, QObject::tr("Timeout") },
		{ StatusCode::TooManyRedirections, QObject::tr("Too Many Redirections") },
		{ StatusCode::TooLongLink, QObject::tr("Too Long Link") },
		{ StatusCode::RedirectLoop, QObject::tr("Redirect Loop") },
		{ StatusCode::UnknownNetworkError, QObject::tr("Unknown Network Error") },
	};

	return s_statusCodeDescriptions;
}

const QString& StatusCodeDescription::description(StatusCode code)
{
	return descriptions()[code];
}

}
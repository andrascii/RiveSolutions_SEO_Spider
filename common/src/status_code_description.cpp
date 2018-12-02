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

		{ StatusCode::MultipleChoices300, "300 Multiple Choices" },
		{ StatusCode::MovedPermanently301, "301 Moved Permanently" },
		{ StatusCode::MovedTemporarily302, "302 Moved Temporarily" },
		{ StatusCode::SeeOther303, "303 See Other" },
		{ StatusCode::NotModified304, "304 Not Modified" },
		{ StatusCode::UseProxy305, "305 Use Proxy" },

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

		{ StatusCode::InternalServerError500, "500 Internal Server Error" },
		{ StatusCode::NotImplemented501, "501 Not Implemented" },
		{ StatusCode::BadGateway502, "502 Bad Gateway" },
		{ StatusCode::ServiceUnavailable503, "503 Service Unavailable" },
		{ StatusCode::GatewayTimeout504, "504 Gateway Timeout" },
		{ StatusCode::HttpVersionNotSupported505, "505 Http Version Not Supported" },

		{ StatusCode::BlockedByRobotsTxt, QObject::tr("Blocked By robots.txt") },
		{ StatusCode::BlockedByMetaRobots, QObject::tr("Blocked By Meta Robots") },
		{ StatusCode::Timeout, QObject::tr("Timeout") },
		{ StatusCode::TooManyRedirections, QObject::tr("Too Many Redirections") },
		{ StatusCode::TooLongLInk, QObject::tr("Too Long Link") },
		{ StatusCode::UnknownNetworkError, QObject::tr("Unknown Network Error") },
	};

	return s_statusCodeDescriptions;
}

const QString& StatusCodeDescription::description(StatusCode code)
{
	return descriptions()[code];
}

}
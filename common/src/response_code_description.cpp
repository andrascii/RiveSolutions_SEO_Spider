#include "response_code_description.h"

namespace
{

using namespace Common;

QMap<ResponseCode, QString> s_responseCodeDescriptions
{
	{ ResponseCode::Continue100, "100 Continue" },
	{ ResponseCode::SwitchingProtocol101, "101 Switching Protocol" },

	{ ResponseCode::Ok200, "200 OK" },
	{ ResponseCode::Created201, "201 Created" },
	{ ResponseCode::Accepted202, "202 Accepted" },
	{ ResponseCode::NonAuthoritativeInformation203, "203 Non Authoritative Information" },
	{ ResponseCode::NoContent204, "204 No Content" },
	{ ResponseCode::ResetContent205, "205 Reset Content" },
	{ ResponseCode::PartialContent206, "206 Partial Content" },

	{ ResponseCode::MultipleChoices300, "300 Multiple Choices" },
	{ ResponseCode::MovedPermanently301, "301 Moved Permanently" },
	{ ResponseCode::MovedTemporarily302, "302 Moved Temporarily" },
	{ ResponseCode::SeeOther303, "303 See Other" },
	{ ResponseCode::NotModified304, "304 Not Modified" },
	{ ResponseCode::UseProxy305, "305 Use Proxy" },

	{ ResponseCode::BadRequest400, "400 Bad Request" },
	{ ResponseCode::Unauthorized401, "401 Unauthorized" },
	{ ResponseCode::PaymentRequired402, "402 Payment Required" },
	{ ResponseCode::Forbidden403, "403 Forbidden" },
	{ ResponseCode::NotFound404, "404 Not Found" },
	{ ResponseCode::MethodNotAllowed405, "405 Method Not Allowed" },
	{ ResponseCode::NonAcceptable406, "406 Non Acceptable" },
	{ ResponseCode::ProxyAuthenticationRequired407, "407 Proxy Authentication Required" },
	{ ResponseCode::RequestTimeout408, "408 Request Timeout" },
	{ ResponseCode::Conflict409, "409 Conflict" },
	{ ResponseCode::Gone410, "410 Gone" },
	{ ResponseCode::LengthRequired411, "411 Length Required" },
	{ ResponseCode::PreconditionFailed412, "412 Precondition Failed" },
	{ ResponseCode::RequestEntityTooLarge413, "413 Request Entity Too Large" },
	{ ResponseCode::RequestUriTooLong414, "414 Request Uri Too Long" },
	{ ResponseCode::UnsupportedMediaType415, "415 Unsupported Media Type" },

	{ ResponseCode::InternalServerError500, "500 Internal Server Error" },
	{ ResponseCode::NotImplemented501, "501 Not Implemented" },
	{ ResponseCode::BadGateway502, "502 Bad Gateway" },
	{ ResponseCode::ServiceUnavailable503, "503 Service Unavailable" },
	{ ResponseCode::GatewayTimeout504, "504 Gateway Timeout" },
	{ ResponseCode::HttpVersionNotSupported505, "505 Http Version Not Supported" }
};

}

namespace Common
{

const QString& ResponseCodeDescription::description(ResponseCode code)
{
	return s_responseCodeDescriptions[code];
}

}
#pragma once

namespace Common
{

enum class StatusCode
{
	Undefined,

	Continue100 = 100,
	SwitchingProtocol101,
	
	Ok200 = 200,
	Created201,
	Accepted202,
	NonAuthoritativeInformation203,
	NoContent204,
	ResetContent205,
	PartialContent206,
	
	MultipleChoices300 = 300,
	MovedPermanently301,
	MovedTemporarily302,
	SeeOther303,
	NotModified304,
	UseProxy305,
	
	BadRequest400 = 400,
	Unauthorized401,
	PaymentRequired402,
	Forbidden403,
	NotFound404,
	MethodNotAllowed405,
	NonAcceptable406,
	ProxyAuthenticationRequired407,
	RequestTimeout408,
	Conflict409,
	Gone410,
	LengthRequired411,
	PreconditionFailed412,
	RequestEntityTooLarge413,
	RequestUriTooLong414,
	UnsupportedMediaType415,
	
	InternalServerError500 = 500,
	NotImplemented501,
	BadGateway502,
	ServiceUnavailable503,
	GatewayTimeout504,
	HttpVersionNotSupported505,

	// custom
	BlockedByRobotsTxt,
	BlockedByMetaRobots,
	Timeout,
	UnknownNetworkError
};

}
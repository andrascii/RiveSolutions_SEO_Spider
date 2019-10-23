#pragma once

namespace Common
{

enum class StatusCode
{
	Undefined,

	Continue100 = 100,
	SwitchingProtocol101,
	Processing102,

	Ok200 = 200,
	Created201,
	Accepted202,
	NonAuthoritativeInformation203,
	NoContent204,
	ResetContent205,
	PartialContent206,
	MultiStatus207,
	AlreadyReported208,
	ImUsed226 = 226,

	MultipleChoices300 = 300,
	MovedPermanently301,
	MovedTemporarily302,
	SeeOther303,
	NotModified304,
	UseProxy305,
	TemporaryRedirect307 = 307,
	PermanentRedirect308,

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
	RangeNotSatisfiable416,
	ExpectationFailed417,
	ImTeapot418,
	AuthenticationTimeout419,
	MisdirectedRequest421 = 421,
	UnprocessableEntity422,
	Locked423,
	FailedDependency424,
	UpgradeRequired426 = 426,
	PreconditionRequired428 = 428,
	TooManyRequests429,
	RequestHeaderFieldsTooLarge431 = 431,
	RetryWith449 = 449,
	UnavailableForLegalReasons451 = 451,
	BadSendedRequest452,
	ClientClosedRequest499 = 499,

	InternalServerError500 = 500,
	NotImplemented501,
	BadGateway502,
	ServiceUnavailable503,
	GatewayTimeout504,
	HttpVersionNotSupported505,
	VariantAlsoNegotiates506,
	InsufficientStorage507,
	LoopDetected508,
	BandwidthLimitExceeded509,
	NotExtended510,
	NetworkAuthenticationRequired511,
	UnknownError520 = 520,
	WebServerIsDown521,
	ConnectionTimedOut522,
	OriginIsUnreachable523,
	ATimeoutOccurred524,
	SslHandshakeFailed525,
	InvalidSslCertificate526,

	// custom
	BlockedByRobotsTxt,
	BlockedByMetaRobots,
	Timeout,
	TooManyRedirections,
	TooLongLink,
	RedirectLoop,
	UnknownNetworkError
};

}
Q_DECLARE_METATYPE(Common::StatusCode)
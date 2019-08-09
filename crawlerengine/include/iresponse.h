#pragma once

namespace CrawlerEngine
{

enum class ResponseType
{
	ResponseTypeUnknown,
	ResponseTypeDownload,
	ResponseTypeDownloadProgress,
	ResponseTypeUploadProgress,
	ResponseTypeGetHostInfo,
	ResponseTypeTaskResult,
	ResponseTypeCheck404IsProper,
	ResponseSetSerialNumber,
	ResponseGetSerialNumberData,
	ResponseGetSerialNumberState,
	ResponseTakeScreenshot,
	ResponseResetConnections
};

#define DECLARE_RESPONSE_STATIC_TYPE(Type) \
	static CrawlerEngine::ResponseType responseStaticType();

#define DEFINE_RESPONSE_STATIC_TYPE(ObjectType, Type) \
	CrawlerEngine::ResponseType ObjectType::responseStaticType() \
	{ \
		return Type; \
	}

#define DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Type) \
	static CrawlerEngine::ResponseType responseStaticType() \
	{ \
		return Type; \
	}

class IResponse
{
public:
	virtual ~IResponse() = default;
	virtual ResponseType type() const noexcept = 0;
};

using IResponseSharedPtr = std::shared_ptr<IResponse>;

}

Q_DECLARE_METATYPE(CrawlerEngine::IResponseSharedPtr)

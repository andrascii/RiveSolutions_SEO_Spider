#pragma once

namespace Common
{

enum class ResponseType
{
	ResponseTypeUnknown,
	ResponseTypeDownload,
	ResponseTypeGetHostInfo
};

#define DECLARE_RESPONSE_STATIC_TYPE(Type) \
	static Common::ResponseType responseStaticType();

#define DEFINE_RESPONSE_STATIC_TYPE(ObjectType, Type) \
	Common::ResponseType ObjectType::responseStaticType() \
	{ \
		return Type; \
	}

#define DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Type) \
	static Common::ResponseType responseStaticType() \
	{ \
		return Type; \
	}

struct IResponse
{
	virtual ResponseType type() const noexcept = 0;
};

using IResponseSharedPtr = std::shared_ptr<IResponse>;
Q_DECLARE_METATYPE(IResponseSharedPtr)

}

#pragma once

namespace Common
{

enum class ResponseType
{
	ResponseTypeUnknown,
	LoadResponseType
};

#define DECLARE_RESPONSE_STATIC_TYPE(Type) \
	static ResponseType responseStaticType();

#define DEFINE_RESPONSE_STATIC_TYPE(ObjectType, Type) \
	ResponseType ObjectType::responseStaticType() \
	{ \
		return Type; \
	}

class IResponse
{
public:
	virtual ResponseType type() const noexcept = 0;
};

using IResponseSharedPtr = std::shared_ptr<IResponse>;
Q_DECLARE_METATYPE(IResponseSharedPtr)

}

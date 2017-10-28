#pragma once

#define DEFINE_METHOD_CHECK(Method)																								\
namespace MetaHelpers {																											\
template <typename ObjectType, typename... ParamTypes>																			\
YesType& hasMethodHelper_##Method(decltype(std::declval<ObjectType&>().Method(std::declval<ParamTypes&>()...))* p);				\
																																\
template <typename...>																											\
NoType& hasMethodHelper_##Method(...);																							\
																																\
template <typename ObjectType, typename... ParamTypes>																			\
struct HasMethod_##Method																										\
	: std::integral_constant<bool, sizeof(hasMethodHelper_##Method<ObjectType, ParamTypes...>(nullptr)) == sizeof(YesType)>		\
{																																\
};																																\
}																																

namespace MetaHelpers
{

using YesType = char;
using NoType = char[2];

}
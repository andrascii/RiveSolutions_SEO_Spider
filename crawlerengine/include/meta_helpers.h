#pragma once

#define DEFINE_METHOD_CHECK(Method)																								\
namespace MetaHelpers {																											\
template <typename ObjectType, typename... ParamTypes>																			\
YesType& hasMethodHelper_##Method(decltype(std::declval<ObjectType&>().Method(std::declval<ParamTypes>()...))* p);				\
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

template <template <typename> class>
constexpr bool isAllOf()
{
	return true;
}

template <template <typename> class Pred, typename Head, typename... Tail>
constexpr bool isAllOf()
{
	return Pred<Head>::value && isAllOf<Pred, Tail...>();
}

// Calls for each argument from argument packet passed functor.
template <typename F, typename Head>
void callForAllArgumentsInPack(F f, Head&& head)
{
	f(head);
}

template <typename F, typename Head, typename... Tail>
void callForAllArgumentsInPack(F f, Head&& head, Tail&&... tail)
{
	f(head);
	callForAllArgumentsInPack(f, std::forward<Tail>(tail)...);
}

// Calls for each argument from argument packet passed functor.
// Returns true if for all passed parameters functor returns true.
template <typename F, typename Head>
bool isEachArgument(F f, Head&& head)
{
	return f(head);
}

template <typename F, typename Head, typename... Tail>
bool isEachArgument(F f, Head&& head, Tail&&... tail)
{
	return f(head) && isEachArgument(f, std::forward<Tail>(tail)...);
}

}
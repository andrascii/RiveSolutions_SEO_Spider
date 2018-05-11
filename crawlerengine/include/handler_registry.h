#pragma once

#include "irequest.h"
#include "meta_helpers.h"

DEFINE_METHOD_CHECK(handleRequest)
DEFINE_METHOD_CHECK(stopRequestHandling)

namespace CrawlerEngine
{

template <typename T>
struct RequestTypeHelper
{
	static constexpr bool value = std::is_same<T, RequestType>::value;
};


class HandlerRegistry
{
public:
	static HandlerRegistry& instance();

	template <typename ObjectType, typename... RequestTypes>
	void registrateHandler(ObjectType* handler, RequestTypes&&... requestTypes)
	{
		static_assert(sizeof...(requestTypes) > 0, "Passed empty request types argument list");
		static_assert(MetaHelpers::isAllOf<RequestTypeHelper, RequestTypes...>(), "Each request type parameter must be a RequestType");
		static_assert(std::is_base_of_v<QObject, ObjectType>, "Passed object must be derived from QObject");
		static_assert(MetaHelpers::HasMethod_handleRequest<ObjectType, RequesterSharedPtr>::value, "Passed object does not contain handleRequest function");
		static_assert(MetaHelpers::HasMethod_stopRequestHandling<ObjectType, RequesterSharedPtr>::value, "Passed object does not contain stopRequestHandling function");

		std::lock_guard<std::mutex> locker(m_mutex);

		const auto conditionCheckerFunctor = [this](RequestType requestType)
		{
			return m_handlers.find(requestType) == m_handlers.end();
		};

		ASSERT(MetaHelpers::isEachArgument(conditionCheckerFunctor, std::forward<RequestTypes>(requestTypes)...));

		const auto functor = [this, handler](RequestType requestType)
		{
			m_handlers[requestType] = static_cast<QObject*>(handler);
		};

		MetaHelpers::callForAllArgumentsInPack(functor, std::forward<RequestTypes>(requestTypes)...);
	}

	void unregistrateHandler(QObject* handler);
	void unregisterAll();

	QObject* handlerForRequest(const IRequest& request);
	bool isHandlerExists(QObject* handler) const;

private:
	HandlerRegistry() = default;
	HandlerRegistry(const HandlerRegistry&) = delete;
	HandlerRegistry(HandlerRegistry&&) = delete;

	std::pair<bool, RequestType> requestTypeForValue(QObject* handler) const;

private:
	mutable std::mutex m_mutex;

	std::map<RequestType, QObject*> m_handlers;
};

}

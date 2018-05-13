#pragma once

#include "irequest.h"
#include "iresponse.h"
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

	void addSubscription(std::function<void(const IResponse&)> subscription, QObject* handler, ResponseType responseType);
	bool hasSubscriptionsFor(QObject* handler, ResponseType responseType) const;
	std::vector<std::function<void(const IResponse&)>> subscriptionsFor(QObject* handler, ResponseType responseType) const;

	void unregistrateHandler(QObject* handler);
	void unregisterAll();

	QObject* handlerForRequest(const IRequest& request);
	QObject* handlerForRequest(RequestType requestType);
	bool isHandlerExists(QObject* handler) const;

private:
	HandlerRegistry() = default;
	HandlerRegistry(const HandlerRegistry&) = delete;
	HandlerRegistry(HandlerRegistry&&) = delete;

	std::pair<bool, RequestType> requestTypeForValue(QObject* handler) const;

private:
	mutable std::mutex m_mutex;

	struct SubscriptionKey
	{
		ResponseType responseType;
		QObject* handler;

		friend bool operator==(const SubscriptionKey& lhs, const SubscriptionKey& rhs)
		{
			return lhs.responseType == rhs.responseType && lhs.handler == rhs.handler;
		}
	};

	struct SubscriptionKeyHasher
	{
		size_t operator()(const SubscriptionKey& key) const
		{
			std::hash<int> intHasher;
			std::hash<QObject*> pointerHasher;

			return intHasher(static_cast<int>(key.responseType)) + pointerHasher(key.handler);
		}
	};

	std::map<RequestType, QObject*> m_handlers;
	std::unordered_map<SubscriptionKey, std::vector<std::function<void(const IResponse&)>>, SubscriptionKeyHasher> m_subscriptions;
};

}

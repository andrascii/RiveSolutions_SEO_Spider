#include "handler_registry.h"

namespace CrawlerEngine
{

HandlerRegistry& HandlerRegistry::instance()
{
	static std::unique_ptr<HandlerRegistry> s_handlerRegistry(new HandlerRegistry);
	return *s_handlerRegistry.get();
}

void HandlerRegistry::addSubscription(Subscription subscriber, QObject* handler, ResponseType responseType)
{
	std::lock_guard locker(m_mutex);

	const SubscriptionKey subscriptionKey{ responseType, handler };

	ASSERT(m_subscriptions.find(subscriptionKey) == m_subscriptions.end());

	m_subscriptions[subscriptionKey].push_back(subscriber);
}

bool HandlerRegistry::hasSubscriptionsFor(QObject* handler, ResponseType responseType) const
{
	std::lock_guard locker(m_mutex);

	return m_subscriptions.find(SubscriptionKey{ responseType, handler }) != m_subscriptions.end();
}

std::vector<HandlerRegistry::Subscription> HandlerRegistry::subscriptionsFor(QObject* handler, ResponseType responseType) const
{
	std::lock_guard locker(m_mutex);

	const auto findIter = m_subscriptions.find(SubscriptionKey{ responseType, handler });

	return findIter == m_subscriptions.end() ? std::vector<Subscription>() : findIter->second;
}

void HandlerRegistry::unregistrateHandler(QObject* handler)
{
	std::lock_guard locker(m_mutex);

	const auto [result, requestType] = requestTypeForValue(handler);

	if (!result)
	{
		return;
	}

	m_handlers.erase(requestType);
}

void HandlerRegistry::unregisterAll()
{
	std::lock_guard locker(m_mutex);

	m_handlers.clear();
}

QObject* HandlerRegistry::handlerForRequest(const IRequest& request)
{
	return handlerForRequest(request.requestType());
}

QObject* HandlerRegistry::handlerForRequest(RequestType requestType)
{
	std::lock_guard locker(m_mutex);

	const auto handlerIterator = m_handlers.find(requestType);

	return handlerIterator == m_handlers.end() ? nullptr : handlerIterator->second;
}

bool HandlerRegistry::isHandlerExists(QObject* handler) const
{
	std::lock_guard locker(m_mutex);

	const auto&& [result, requestType] = requestTypeForValue(handler);

	Q_UNUSED(requestType);

	return result;
}

std::pair<bool, RequestType> HandlerRegistry::requestTypeForValue(QObject* handler) const
{
	for (const auto& [key, value] : m_handlers)
	{
		if (value == handler)
		{
			return std::make_pair(true, key);
		}
	}

	return std::make_pair(false, RequestType());
}

}
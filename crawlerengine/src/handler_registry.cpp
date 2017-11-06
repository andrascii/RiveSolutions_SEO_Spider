#include "handler_registry.h"

namespace CrawlerEngine
{

HandlerRegistry& HandlerRegistry::instance()
{
	static std::unique_ptr<HandlerRegistry> s_handlerRegistry(new HandlerRegistry);
	return *s_handlerRegistry.get();
}

void HandlerRegistry::unregistrateHandler(QObject* handler)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	const auto [result, requestType] = requestTypeForValue(handler);

	if (!result)
	{
		return;
	}

	m_handlers.erase(requestType);
}

void HandlerRegistry::unregisterAll()
{
	m_handlers.clear();
}

QObject* HandlerRegistry::handlerForRequest(const IRequest& request)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	const auto handlerIterator = m_handlers.find(request.requestType());

	return handlerIterator == m_handlers.end() ? nullptr : handlerIterator->second;
}

bool HandlerRegistry::isHandlerExists(QObject* handler) const
{
	std::lock_guard<std::mutex> locker(m_mutex);

	const auto& [result, requestType] = requestTypeForValue(handler);

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
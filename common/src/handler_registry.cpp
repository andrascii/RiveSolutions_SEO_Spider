#include "handler_registry.h"

namespace Common
{

HandlerRegistry& HandlerRegistry::instance()
{
	static std::unique_ptr<HandlerRegistry> s_handlerRegistry(new HandlerRegistry);
	return *s_handlerRegistry.get();
}

void HandlerRegistry::registrateHandler(QObject* handler, RequestType requestType)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	ASSERT(m_handlers.find(requestType) == m_handlers.end());

	m_handlers[requestType] = handler;
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

QObject* HandlerRegistry::handlerForRequest(const IRequest& request)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	const auto handlerIterator = m_handlers.find(request.requestType());

	return handlerIterator == m_handlers.end() ? nullptr : handlerIterator->second;
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
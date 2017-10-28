#pragma once

#include "irequest.h"
#include "meta_helpers.h"

DEFINE_METHOD_CHECK(handleRequest)
DEFINE_METHOD_CHECK(stopRequestHandling)

namespace Common
{
	
class HandlerRegistry
{
public:
	static HandlerRegistry& instance();

	template <typename ObjectType>
	void registrateHandler(ObjectType* handler, RequestType requestType)
	{
		static_assert(std::is_base_of_v<QObject, ObjectType>, "Passed object must be derived from QObject");
		static_assert(MetaHelpers::HasMethod_handleRequest<ObjectType, RequesterSharedPtr>::value, "Passed object does not contain handleRequest function");
		static_assert(MetaHelpers::HasMethod_stopRequestHandling<ObjectType, RequesterSharedPtr>::value, "Passed object does not contain stopRequestHandling function");

		std::lock_guard<std::mutex> locker(m_mutex);

		ASSERT(m_handlers.find(requestType) == m_handlers.end());

		m_handlers[requestType] = static_cast<QObject*>(handler);
	}

	void unregistrateHandler(QObject* handler);

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

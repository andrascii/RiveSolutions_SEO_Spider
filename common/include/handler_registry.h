#pragma once

#include "irequest.h"

namespace Common
{

class AbstractHandler;

class HandlerRegistry
{
public:
	static HandlerRegistry& instance();

	void registrateHandler(QObject* handler, RequestType requestType);
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

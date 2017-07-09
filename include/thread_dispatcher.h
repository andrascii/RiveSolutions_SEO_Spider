#pragma once

#include "request_types.h"

namespace QuickieWebBot
{

class Message
{
public:

};

class ThreadDispatcher : public QObject
{
	Q_OBJECT

public:
	ThreadDispatcher(QObject* parent = nullptr);

	Q_INVOKABLE void postMessage() noexcept;

	Q_INVOKABLE void dispatchIncoming() const noexcept;
	Q_INVOKABLE void dispatchOutgoing() const noexcept;

private:
	std::queue<std::reference_wrapper<IRequest>> m_requestQueue;
};

}

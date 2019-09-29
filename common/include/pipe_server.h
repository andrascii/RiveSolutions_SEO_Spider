#pragma once

#include "irpc_server.h"
#include "irpc_socket.h"

namespace Common
{

class PipeServer final
{
public:
	PipeServer();
	~PipeServer();

	template <typename ReceiveType>
	qint64 readData(ReceiveType& r)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return 0;
		}

		return m_socket->readData(reinterpret_cast<char*>(&r), sizeof(r));
	}

	template <typename SendType>
	qint64 writeData(const SendType& s)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return 0;
		}

		return m_socket->writeData(reinterpret_cast<const char*>(&s), sizeof(s));
	}

	qint64 writeData(const char* data, qint64 maxSize);

	template <typename ReceiveType>
	qint64 peekData(ReceiveType& r)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return 0;
		}

		return m_socket->peekData(reinterpret_cast<char*>(&r), sizeof(r));
	}

	template <typename SendType, typename ReceiveType>
	qint64 transactData(const SendType& s, ReceiveType& r)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return 0;
		}

		return m_socket->transactData(reinterpret_cast<const char*>(&s), sizeof(s), reinterpret_cast<char*>(&r), sizeof(r));
	}

	void closeConnection();
	void listen(const QString& channelName);
	bool hasConnection() const;

private:
	mutable std::mutex m_mutex;
	std::shared_ptr<IRpcSocket> m_socket;
	std::shared_ptr<IRpcServer> m_server;
};

}

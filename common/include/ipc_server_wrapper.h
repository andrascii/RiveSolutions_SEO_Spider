#pragma once

#include "ipc_server.h"

namespace Common
{

class IpcSocket;

class IpcServerWrapper final : public QObject
{
	Q_OBJECT

public:
	IpcServerWrapper();
	~IpcServerWrapper();

	template <typename Data>
	void writeData(const Data& data)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return;
		}

		m_socket->writeData(reinterpret_cast<const char*>(&data), sizeof(data));
	}

	template <typename Data>
	qint64 readData(Data& data)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return 0;
		}

		return m_socket->readData(reinterpret_cast<char*>(&data), sizeof(data));
	}

	template <typename SendData, typename ReceiveData>
	void transactData(const SendData& sendData, ReceiveData& receiveData)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return;
		}

		m_socket->transactData(
			reinterpret_cast<const char*>(&sendData),
			sizeof(sendData),
			reinterpret_cast<const char*>(&receiveData),
			sizeof(receiveData)
		);
	}

	template <typename ReceiveData>
	qint64 peekData(ReceiveData& receiveData)
	{
		std::lock_guard locker(m_mutex);

		if (!m_socket || m_socket->isClosed())
		{
			return -1;
		}

		const qint64 size = m_socket->peekData(reinterpret_cast<char*>(&receiveData), sizeof(receiveData));

		return size;
	}

	bool hasConnection() const;

public slots:
	void listen(const QString& name);
	void closeConnection();

signals:
	void connectionClosed();

private:
	mutable std::mutex m_mutex;
	Common::IpcServer m_server;
	Common::IpcSocket* m_socket;
};

}
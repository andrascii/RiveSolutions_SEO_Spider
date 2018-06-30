#include "ipc_server_wrapper.h"

namespace Common
{

void IpcServerWrapper::listen(const QString& name)
{
	closeConnection();

	if (m_server.listen(name, QIODevice::ReadWrite))
	{
		std::lock_guard locker(m_mutex);

		m_socket = m_server.nextPendingConnection();
	}
}

void IpcServerWrapper::closeConnection()
{
	std::lock_guard locker(m_mutex);

	if (m_socket && !m_socket->isClosed())
	{
		m_socket->disconnectFromServer();
	}

	m_socket = nullptr;

	emit connectionClosed();
}

IpcServerWrapper::IpcServerWrapper()
	: m_socket(nullptr)
{
}

IpcServerWrapper::~IpcServerWrapper()
{
	closeConnection();
}

bool IpcServerWrapper::hasConnection() const
{
	return m_socket && !m_socket->isClosed();
}

}
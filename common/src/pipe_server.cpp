#include "stdafx.h"
#include "pipe_server.h"
#include "rpc_factory.h"

namespace Common
{

PipeServer::PipeServer()
    : m_server(RpcFactory::createRpcServer())
{
}

PipeServer::~PipeServer()
{
	closeConnection();
}

void PipeServer::closeConnection()
{
	std::lock_guard locker(m_mutex);

	if (m_socket && !m_socket->isClosed())
	{
		m_socket->disconnectFromServer();
	}
}

void PipeServer::listen(const QString& channelName)
{
	if (m_server->listen(channelName))
	{
		m_socket = m_server->nextPendingConnection();
	}
}

bool PipeServer::hasConnection() const
{
	std::lock_guard locker(m_mutex);

	return m_socket && !m_socket->isClosed();
}

qint64 PipeServer::writeData(const char* data, qint64 maxSize)
{
	std::lock_guard locker(m_mutex);

	if (!m_socket || m_socket->isClosed())
	{
		return 0;
	}

	return m_socket->writeData(data, maxSize);
}

}

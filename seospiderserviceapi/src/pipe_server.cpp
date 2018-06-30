#include "pipe_server.h"

namespace SeoSpiderServiceApi
{

PipeServer::PipeServer()
	: m_socket(nullptr)
{
	if (m_server.listen(QString("seospiderserviceapi_log_channel"), QIODevice::ReadWrite))
	{
		m_socket = m_server.nextPendingConnection();
	}
}

PipeServer::~PipeServer()
{
	closeConnection();
}

void PipeServer::closeConnection()
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (m_socket && !m_socket->isClosed())
	{
		m_socket->disconnectFromServer();
	}
}

qint64 PipeServer::readData(char* data, qint64 maxSize)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!m_socket || m_socket->isClosed())
	{
		return 0;
	}

	return m_socket->readData(data, maxSize);
}

qint64 PipeServer::writeData(const char* data, qint64 maxSize)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!m_socket || m_socket->isClosed())
	{
		return 0;
	}

	return m_socket->writeData(data, maxSize);
}

qint64 PipeServer::peekData(char* data, qint64 maxSize)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!m_socket || m_socket->isClosed())
	{
		return 0;
	}

	return m_socket->peekData(data, maxSize);
}

qint64 PipeServer::transactData(const char* inData, qint64 inSize, char* outData, int outSize)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	if (!m_socket || m_socket->isClosed())
	{
		return 0;
	}

	return m_socket->transactData(inData, inSize, outData, outSize);
}

}
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
	std::lock_guard<std::mutex> locker(m_mutex);

	if (m_socket && !m_socket->isClosed())
	{
		m_socket->disconnectFromServer();
	}
}

void PipeServer::logMessage(const QString& message, SeverityLevel severityLevel)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	Common::PipeMessage pipeMessage;

	pipeMessage.severityLevel = static_cast<std::uint64_t>(severityLevel);

	const QByteArray messageBytes = message.toUtf8();
	std::strcpy(pipeMessage.message, messageBytes.data());

	logMessage(pipeMessage);
}

void PipeServer::logMessage(const Common::PipeMessage& message)
{
	if (!m_socket || m_socket->isClosed())
	{
		m_messages.push_back(message);

		return;
	}

	m_socket->writeData(reinterpret_cast<const char*>(&message), sizeof(message));
}

}
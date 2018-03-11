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

void PipeServer::logMessage(
	Common::PipeMessage::Type type,
	Common::SeverityLevel level,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	std::lock_guard<std::mutex> locker(m_mutex);

	Common::PipeMessage pipeMessage;

	pipeMessage.type = type;
	pipeMessage.severityLevel = static_cast<std::uint64_t>(level);
	pipeMessage.threadId = threadId;
	pipeMessage.line = line;
	std::strcpy(pipeMessage.file, file);
	std::strcpy(pipeMessage.function, function);
	std::strcpy(pipeMessage.message, message);

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
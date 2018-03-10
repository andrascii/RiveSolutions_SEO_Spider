#pragma once

#include "ipc_server.h"
#include "pipe_message.h"

namespace SeoSpiderServiceApi
{

class PipeServer final
{
public:
	PipeServer();
	~PipeServer();

	void logMessage(const QString& message, SeverityLevel severityLevel);

private:
	void logMessage(const Common::PipeMessage& message);

private:
	mutable std::mutex m_mutex;
	std::deque<Common::PipeMessage> m_messages;
	Common::IpcServer m_server;
	Common::IpcSocket* m_socket;
};

}
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

	void logMessage(
		Common::PipeMessage::Type type,
		Common::SeverityLevel level,
		std::uint64_t threadId,
		std::uint64_t line,
		const char* file,
		const char* function,
		const char* message
	);

private:
	void logMessage(const Common::PipeMessage& message);

private:
	mutable std::mutex m_mutex;
	std::deque<Common::PipeMessage> m_messages;
	Common::IpcServer m_server;
	Common::IpcSocket* m_socket;
};

}
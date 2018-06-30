#pragma once

#include "ipc_server.h"

namespace SeoSpiderServiceApi
{

class PipeServer final
{
public:
	PipeServer();
	~PipeServer();

	qint64 readData(char* data, qint64 maxSize);
	qint64 writeData(const char* data, qint64 maxSize);
	qint64 peekData(char* data, qint64 maxSize);
	qint64 transactData(const char* inData, qint64 inSize, char* outData, int outSize);
	void closeConnection();

private:
	mutable std::mutex m_mutex;
	Common::IpcServer m_server;
	Common::IpcSocket* m_socket;
};

}
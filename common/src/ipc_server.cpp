#include "ipc_server.h"

namespace
{

constexpr qint64 c_connectionTimeout = 5000;
constexpr qint64 c_datagramSizeLimit = 8192;

}

namespace Common
{

IpcServer::IpcServer()
	: m_descriptor(0)
	, m_openMode(QIODevice::NotOpen)
{
}

bool IpcServer::listen(const QString& name, QIODevice::OpenModeFlag mode)
{
	static const quint64 s_pipeBufferSize = 1024 * 1024;

	const QString pipePath("\\\\.\\pipe\\");

	m_serverName = name;
	m_fullServerName = name.startsWith(pipePath) ? name : pipePath + name;

	DWORD access = QIODevice::ReadOnly == mode ? PIPE_ACCESS_INBOUND :
		QIODevice::WriteOnly == mode ? PIPE_ACCESS_OUTBOUND : PIPE_ACCESS_DUPLEX;

	HANDLE pipe = CreateNamedPipeW(reinterpret_cast<LPCWSTR>(m_fullServerName.utf16()),
		access,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		s_pipeBufferSize,
		s_pipeBufferSize,		// input buffer size
		c_connectionTimeout,	// client time-out 
		NULL					// default security attribute 
	);

	if (INVALID_HANDLE_VALUE == pipe)
	{
		m_errorString = QString("create connection failed (%1)").arg(GetLastError());
		return false;
	}

	m_descriptor = quintptr(pipe);
	m_openMode = mode;

	return true;
}

IpcSocket* IpcServer::nextPendingConnection()
{
	//TODO: use overlapped io
	Q_ASSERT(m_descriptor);

	if (!ConnectNamedPipe(HANDLE(m_descriptor), 0) && ERROR_PIPE_CONNECTED != GetLastError())
	{
		m_errorString = QString("accept connection failed (%1)").arg(GetLastError());
		CloseHandle(HANDLE(m_descriptor));
		return 0;
	}

	m_socket.reset(new IpcSocket(m_descriptor, m_openMode));
	return m_socket.get();
}

}

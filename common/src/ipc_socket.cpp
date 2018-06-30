#include "ipc_socket.h"

namespace Common
{

IpcSocket::IpcSocket()
	: m_descriptor(0)
	, m_openMode(QIODevice::NotOpen)
{
}

IpcSocket::IpcSocket(quintptr descriptor, QIODevice::OpenModeFlag mode)
	: m_descriptor(descriptor)
	, m_openMode(mode)
{
}

IpcSocket::~IpcSocket()
{
	disconnectFromServer();
}

bool IpcSocket::connectToServer(const QString& name, QIODevice::OpenModeFlag mode)
{
	Q_ASSERT(0 == m_descriptor);

	if (name.isEmpty())
	{
		m_errorString = QString("empty server name");
		return false;
	}

	const DWORD flags = QIODevice::ReadOnly == mode ? GENERIC_READ :
		QIODevice::WriteOnly == mode ? GENERIC_WRITE : GENERIC_READ | GENERIC_WRITE;

	const QString pipePath("\\\\.\\pipe\\");

	m_fullServerName = name.startsWith(pipePath) ? name : pipePath + name;

	HANDLE pipe = CreateFileW(reinterpret_cast<LPCWSTR>(m_fullServerName.utf16()), flags, 0, 0, OPEN_EXISTING, 0, 0);

	if (INVALID_HANDLE_VALUE == pipe)
	{
		m_errorString = QString("can't open connection (%1)").arg(GetLastError());
		return false;
	}

	DWORD pipeMode = PIPE_READMODE_MESSAGE | PIPE_WAIT;

	if (!SetNamedPipeHandleState(pipe, &pipeMode, 0, 0))
	{
		m_errorString = QString("can't change socket descriptor mode (%1)").arg(GetLastError());
		CloseHandle(pipe);
		return false;
	}

	m_openMode = mode;
	m_serverName = name;
	m_descriptor = quintptr(pipe);

	return true;
}

void IpcSocket::disconnectFromServer()
{
	if (m_descriptor)
	{
		CloseHandle(HANDLE(m_descriptor));
		m_openMode = QIODevice::NotOpen;
		m_descriptor = 0;
	}
}

qint64 IpcSocket::readData(char* data, qint64 maxSize)
{
	Q_ASSERT(QIODevice::ReadOnly & m_openMode);

	DWORD bytes = 0;

	if (!ReadFile(HANDLE(m_descriptor), data, DWORD(maxSize), &bytes, 0))
	{
		m_errorString = QString("read data failed (%1)").arg(GetLastError());
	}

	return bytes;
}

qint64 IpcSocket::writeData(const char* data, qint64 maxSize)
{
	Q_ASSERT(QIODevice::WriteOnly & m_openMode);

	DWORD bytes = 0;

	if (!WriteFile(HANDLE(m_descriptor), data, DWORD(maxSize), &bytes, 0))
	{
		m_errorString = QString("write data failed (%1)").arg(GetLastError());
	}

	return bytes;
}

qint64 IpcSocket::peekData(char* data, qint64 maxSize)
{
	DWORD bytesread = 0;
	DWORD totalbytes = 0;
	DWORD messagebytes = 0;

	if (!PeekNamedPipe(HANDLE(m_descriptor), data, DWORD(maxSize), &bytesread, &totalbytes, &messagebytes))
	{
		m_errorString = QString("peek data failed (%1)").arg(GetLastError());
		return -1;
	}

	return bytesread;
}

qint64 IpcSocket::transactData(const char* inData, qint64 inSize, char* outData, int outSize)
{
	Q_ASSERT(QIODevice::ReadWrite == m_openMode);

	DWORD bytes = 0;

	if (!TransactNamedPipe(HANDLE(m_descriptor), (void*)inData, DWORD(inSize), outData, outSize, &bytes, 0))
	{
		m_errorString = QString("data transaction failed (%1)").arg(GetLastError());
	}

	return bytes;
}

bool IpcSocket::isClosed()
{
	char c;
	return 0 > peekData(&c, 0) && ERROR_BROKEN_PIPE == GetLastError();
}

QIODevice::OpenModeFlag IpcSocket::openMode() const
{
	return m_openMode;
}

}

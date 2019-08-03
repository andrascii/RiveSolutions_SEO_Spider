#pragma once
#include "stdafx.h"

#include "ipc_socket.h"

namespace Common
{

class IpcServer
{
public:
	IpcServer();

	bool listen(const QString& name, QIODevice::OpenModeFlag mode);
	IpcSocket* nextPendingConnection();

	QString serverName() const { return m_serverName; }
	QString fullServerName() const { return m_serverName; }
	QString errorString() const { return m_errorString; }

private:
	QString m_serverName;
	QString m_fullServerName;
	QString m_errorString;
	quintptr m_descriptor;
	std::unique_ptr<IpcSocket> m_socket;
	QIODevice::OpenModeFlag m_openMode;
};

}

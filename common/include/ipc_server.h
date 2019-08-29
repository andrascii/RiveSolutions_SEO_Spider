#pragma once

#include "irpc_socket.h"
#include "irpc_server.h"

namespace Common
{

class IpcServer : public IRpcServer
{
public:
	IpcServer();

	virtual bool listen(const QString& name) override;
    virtual std::shared_ptr<IRpcSocket> nextPendingConnection() override;

	virtual QString serverName() const override { return m_serverName; }
	virtual QString fullServerName() const override { return m_serverName; }
	virtual QString errorString() const override { return m_errorString; }

private:
	QString m_serverName;
	QString m_fullServerName;
	QString m_errorString;
	quintptr m_descriptor;
    std::unique_ptr<IRpcSocket> m_socket;
};

}

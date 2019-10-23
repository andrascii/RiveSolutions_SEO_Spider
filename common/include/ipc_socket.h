#pragma once

#include "irpc_socket.h"

namespace Common
{

class IpcSocket : public IRpcSocket
{
public:
	IpcSocket();
    IpcSocket(quintptr descriptor);
	~IpcSocket();

	virtual bool connectToServer(const QString& name) override;
	virtual void disconnectFromServer() override;
	virtual QString serverName() const override { return m_serverName; }
	virtual QString fullServerName() const override { return m_fullServerName; }
	virtual qint64 readData(char* data, qint64 maxSize) override;
	virtual qint64 writeData(const char* data, qint64 maxSize) override;
	virtual qint64 peekData(char* data, qint64 maxSize) override;
	virtual qint64 transactData(const char* inData, qint64 inSize, char* outData, int outSize) override;
	virtual QString errorString() const override { return m_errorString; }
	virtual bool isClosed() override;

private:
	quintptr m_descriptor;
	QString m_serverName;
	QString m_fullServerName;
	QString m_errorString;
};

}

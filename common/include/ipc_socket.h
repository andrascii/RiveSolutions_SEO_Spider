#pragma once

namespace Common
{

class IpcSocket
{
public:
	IpcSocket();
	IpcSocket(quintptr descriptor, QIODevice::OpenModeFlag mode);
	~IpcSocket();
	bool connectToServer(const QString& name, QIODevice::OpenModeFlag mode);
	void disconnectFromServer();
	QString serverName() const { return m_serverName; }
	QString fullServerName() const { return m_fullServerName; }
	qint64 readData(char* data, qint64 maxSize);
	qint64 writeData(const char* data, qint64 maxSize);
	qint64 peekData(char* data, qint64 maxSize);
	qint64 transactData(const char* inData, qint64 inSize, char* outData, int outSize);
	QString errorString() const { return m_errorString; }
	bool isClosed();
	QIODevice::OpenModeFlag openMode() const;

private:
	quintptr m_descriptor;
	QString m_serverName;
	QString m_fullServerName;
	QIODevice::OpenModeFlag m_openMode;
	QString m_errorString;
};

}

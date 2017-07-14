#pragma once

namespace QuickieWebBot 
{

class LoggerConnectionServiceApi : public QObject
{
	Q_OBJECT

public:
	enum MessageType
	{
		InformationMessageType,
		DebugMessageType,
		WarningMessageType,
		ErrorMessageType
	};

public:
	static LoggerConnectionServiceApi* instance();

	void log(MessageType type, QString tag, QString text, QString func, QString file, int line) const noexcept;

private:
	LoggerConnectionServiceApi(QObject* parent = 0);

private:
	mutable std::mutex m_mutex;
	QProcess* m_loggerProc;
	QTcpSocket* m_socket;
	QTimer m_connectionTimer;
	int m_blockSize;
	bool m_connected;
};

}
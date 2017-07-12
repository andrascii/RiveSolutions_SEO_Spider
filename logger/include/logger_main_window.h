#pragma once

#include "ui_logger_main_window.h"

class LoggerMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoggerMainWindow(QWidget *parent = 0);

	enum MessageType
	{
		InformationMessageType,
		DebugMessageType,
		WarningMessageType,
		ErrorMessageType
	};

private:
	void init();

private slots:
	void slotNewConnection();
	void slotReadyRead();

private:
	Ui::MainWindow ui;
	QTcpServer* m_server;
	QString message;

	//QLocalSocket* m_socket;
	//QString m_currentFortune;
	quint16 m_blockSize;

	struct incomingMessage
	{
		MessageType type;
		QString dateTime;
		QString source;
		QString tag;
		QString text;
		
		incomingMessage(QStringList message)
		{
			type = static_cast<MessageType>(message[0][0].toLatin1() - 48);
			dateTime = message[1];
			source = message[2];
			tag = message[3];
			text = message[4];
		}

		QString toString()
		{
			return QString(dateTime + ' ' + source + ": " + tag + " \"" + text + '\"');
		}
	};

	QList<incomingMessage> m_incomingMessages;
	
};
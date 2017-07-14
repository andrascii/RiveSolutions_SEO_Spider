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
		QString file;
		QString line;
		
		incomingMessage(QStringList message)
		{
			type = static_cast<MessageType>(message[0][0].toLatin1() - 48);
			dateTime = message[1];
			source = message[2];
			tag = message[3];
			text = message[4];
			file = message[5];
			line = message[6];
		}

		QString toString()
		{
			//return QString(dateTime + ' ' + source + ": " + tag + " \"" + text + '\"');
			return QString("%1 %2(%3) %4: %5 \"%6\"")
				.arg(dateTime)
				.arg(file)
				.arg(line)
				.arg(source)
				.arg(tag)
				.arg(text);
		}
	};

	QList<incomingMessage> m_incomingMessages;
	
};
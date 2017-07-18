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
		ErrorMessageType,
		NoType
	};

private:
	struct incomingMessage
	{
		MessageType type;
		QString dateTime;
		QString source;
		QString text;
		QString file;
		QString line;

		incomingMessage(QStringList message)
		{
			type = static_cast<MessageType>(message[0][0].toLatin1() - 48);
			dateTime = message[1];
			source = message[2];
			file = message[3];
			line = message[4];
			text = message[5];
		}

		QString toString()
		{
			return QString("%1 %2(%3) %4: \"%5\"")
				.arg(dateTime)
				.arg(file)
				.arg(line)
				.arg(source)
				.arg(text);
		}
	};

	void init();
	void redrawList();
	QColor identifyMessageColor(MessageType type);
	MessageType typeFromString(const QString& type);

signals:
	void messageAppendedToList();

private slots:
	void slotNewConnection();
	void slotReadyRead();
	void slotApplyTypeFilter(const QString& type);
	void slotTryToShowNewMessage();

private:
	Ui::MainWindow ui;

	QTcpServer* m_server;
	quint16 m_blockSize;

	QList<incomingMessage> m_incomingMessages;
	MessageType m_currentTypeFilter;
};
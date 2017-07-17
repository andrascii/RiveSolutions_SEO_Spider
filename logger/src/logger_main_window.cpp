#include "logger_main_window.h"
#include <Windows.h>

LoggerMainWindow::LoggerMainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_blockSize(0)
{
	ui.setupUi(this);
	m_server = new QTcpServer(this);

	if (!m_server->listen(QHostAddress::LocalHost,12345))
	{
		QMessageBox::critical(this, tr("Logger Server"), tr("Unable to start the server: %1.").arg(m_server->errorString()));

		m_server->close();
		return;
	}

	VERIFY(connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection())));

	ui.textBrowser->setFontPointSize(12);
}

void LoggerMainWindow::slotNewConnection()
{
	QTcpSocket* clientSocket = m_server->nextPendingConnection();

	VERIFY(connect(clientSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead())));
	VERIFY(connect(this, SIGNAL(messageAppendedToList()), this, SLOT(appendNewMessage())));
	ui.textBrowser->append("connected");
}

void LoggerMainWindow::appendNewMessage()
{
	ui.textBrowser->setTextColor(identifyMessageColor(m_incomingMessages.last())); 
	ui.textBrowser->append(m_incomingMessages.last().toString());
}

void LoggerMainWindow::redrawList()
{
	for (auto item : m_incomingMessages)
	{
		ui.textBrowser->append(item.toString());
	}
}

QColor LoggerMainWindow::identifyMessageColor(const incomingMessage& message)
{
	switch (message.type)
	{
	case InformationMessageType:
		return QColor(0, 0, 0); break;
	case DebugMessageType:
		return QColor(0, 0, 255); break;
	case WarningMessageType:
		return QColor(255, 165, 0); break;
	case ErrorMessageType:
		return QColor(255, 0, 0); break;
	default:
		return QColor(0, 0, 0); break;
	}
}

void LoggerMainWindow::slotReadyRead()
{
	QTcpSocket* clientSocket = (QTcpSocket*)sender();
	QDataStream in(clientSocket);
	in.setVersion(QDataStream::Qt_4_0);

	for (;;)
	{
		if (!m_blockSize)
		{
			if (clientSocket->bytesAvailable() < sizeof(quint16))
			{
				break;
			}

			in >> m_blockSize;
		}
	
		if (clientSocket->bytesAvailable() < m_blockSize)
		{
			break;
		}
		QString incomingMessage;

		in >> incomingMessage;

		m_incomingMessages.append(incomingMessage.split('|'));
		emit messageAppendedToList();
		//appendNewMessage();

		m_blockSize = 0;
	}
}
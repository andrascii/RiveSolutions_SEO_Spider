#include "logger_main_window.h"

LoggerMainWindow::LoggerMainWindow(QWidget* parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	m_server = new QTcpServer(this);

	if (!m_server->listen(QHostAddress::LocalHost,12345))
	{
		QMessageBox::critical(this, tr("Logger Server"), tr("Unable to start the server: %1.").arg(m_server->errorString()));

		m_server->close();
		return;
	}

	m_blockSize = 0;
	connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void LoggerMainWindow::slotNewConnection()
{
	QTcpSocket* clientSocket = m_server->nextPendingConnection();

	VERIFY(connect(clientSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead())));

	ui.textBrowser->append("connected");
}

void LoggerMainWindow::slotReadyRead()
{
	QTcpSocket* client_socket = (QTcpSocket*)sender();
	QDataStream in(client_socket);
	in.setVersion(QDataStream::Qt_4_0);

	for (;;)
	{
		if (!m_blockSize)
		{
			if (client_socket->bytesAvailable() < sizeof(quint16))
			{
				break;
			}

			in >> m_blockSize;
		}
	
		if (client_socket->bytesAvailable() < m_blockSize)
		{
			break;
		}
		QString str;

		in >> str;

		m_incomingMessages.append(str.split('|'));
	
		switch (m_incomingMessages.last().type)
		{
		case InformationMessageType: 
			ui.textBrowser->setTextColor(QColor(0, 0, 0)); break;
		case DebugMessageType:
			ui.textBrowser->setTextColor(QColor(0, 0, 255)); break;
		case WarningMessageType: 
			ui.textBrowser->setTextColor(QColor(255,165,0)); break;
		case ErrorMessageType: 
			ui.textBrowser->setTextColor(QColor(255, 0, 0)); break;
		}

		ui.textBrowser->append(m_incomingMessages.last().toString());
		m_blockSize = 0;
	}
}
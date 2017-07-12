#include "logger_main_window.h"
#include <Windows.h>

LoggerMainWindow::LoggerMainWindow(QWidget* parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	m_server = new QTcpServer(this);
	if (!m_server->listen(QHostAddress::LocalHost,12345))
	{
		QMessageBox::critical(this, tr("Logger Server"),
			tr("Unable to start the server: %1.").arg(m_server->errorString()));
		m_server->close();
		return;
	}
	//QMessageBox::information(this, tr("Logger Server"),
	//		tr("Server starter"));
	m_blockSize = 0;
	connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void LoggerMainWindow::slotNewConnection()
{
	//while (!IsDebuggerPresent())
	//{
	//}

	QTcpSocket* clientSocket = m_server->nextPendingConnection();
	connect(clientSocket, SIGNAL(readyRead()),
		this, SLOT(slotReadyRead()));
	ui.textBrowser->append("connected");
}

void LoggerMainWindow::slotReadyRead()
{
	ui.textBrowser->append("data recieved");
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
		ui.textBrowser->append(str);
		m_blockSize = 0;
	}
}
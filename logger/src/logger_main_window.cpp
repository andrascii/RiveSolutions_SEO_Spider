#include "logger_main_window.h"

LoggerMainWindow::LoggerMainWindow(QWidget* parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	
	m_server = new QLocalServer(this);
	if (!m_server->listen("Logger"))
	{
		//QMessageBox::critical(this, tr("Logger Server"),
		//	tr("Unable to start the server: %1.").arg(m_server->errorString()));
		close();
		return;
	}
	//QMessageBox::information(this, tr("Logger Server"),
	//		tr("Server starter"));
	
	connect(m_server, &QLocalServer::newConnection, this, &LoggerMainWindow::sendMessage);

}

void LoggerMainWindow::sendMessage()
{
	//QByteArray block;
	//QDataStream out(&block, QIODevice::WriteOnly);
	//out.setVersion(QDataStream::Qt_5_1);
	//out << (quint32)0;
	//out << message;
	//out.device()->seek(0);
	//out << (quint32)(block.size() - sizeof(quint32));
	//
	//QLocalSocket* clientConnection = m_server->nextPendingConnection();
	//connect(clientConnection, SIGNAL(disconnected()),
	//	clientConnection, SLOT(deleteLater()));
	//
	//clientConnection->write(block);
	//clientConnection->flush();
	//clientConnection->disconnectFromServer();
	//m_server->
	ui.textBrowser->append("zalupa");
	quint16 blockSize = 0;
	QLocalSocket* client_socket = m_server->nextPendingConnection();
	QDataStream in(client_socket);
	in.setVersion(QDataStream::Qt_4_0);
	if (client_socket->waitForReadyRead())
	{
		if (blockSize == 0)
		{
			if (client_socket->bytesAvailable() < (int)sizeof(quint16))
				return;
			in >> blockSize;
		}
		if (in.atEnd())
			return;
		QString mes;
		in >> mes;
		ui.textBrowser->append(mes);
	}
}
#include "logger_main_window.h"
#include <Windows.h>

LoggerMainWindow::LoggerMainWindow(QWidget* parent) :
	QMainWindow(parent),
	m_blockSize(0),
	m_currentTypeFilter(NoType)
{
	ui.setupUi(this);
	m_server = new QTcpServer(this);

	//while (!IsDebuggerPresent());

	if (!m_server->listen(QHostAddress::LocalHost,12345))
	{
		QMessageBox::critical(this, tr("Logger Server"), tr("Unable to start the server: %1.").arg(m_server->errorString()));

		m_server->close();
		return;
	}

	VERIFY(connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection())));

	VERIFY(connect(ui.comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotApplyTypeFilter(QString))));
	VERIFY(connect(ui.clearButton, SIGNAL(released()), this, SLOT(slotClearLogger())));
	VERIFY(connect(ui.lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotApplyStringFilter())));	
	
	ui.lineEdit->setClearButtonEnabled(true);
	ui.textBrowser->setFontPointSize(11);
}

void LoggerMainWindow::slotNewConnection()
{
	QTcpSocket* clientSocket = m_server->nextPendingConnection();

	VERIFY(connect(clientSocket, SIGNAL(readyRead()), this, SLOT(slotReadyRead())));
	VERIFY(connect(clientSocket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconected())));
	VERIFY(connect(this, SIGNAL(messageAppendedToList()), this, SLOT(slotTryToShowNewMessage())));

	ui.textBrowser->append("connected");
}

LoggerMainWindow::MessageType LoggerMainWindow::typeFromString(const QString& type)
{
	if (type == "Information")
		return InformationMessageType;
	if (type == "Warning")
		return WarningMessageType;
	if (type == "Errors")
		return ErrorMessageType;
	if (type == "Debug")
		return DebugMessageType;

	return NoType;
}

void LoggerMainWindow::slotApplyStringFilter()
{
	if (m_currentStringFilter != ui.lineEdit->text())
	{
		m_currentStringFilter = ui.lineEdit->text();
		redrawList();
	}
}

void LoggerMainWindow::slotClearLogger()
{
	m_incomingMessages.clear();

	ui.textBrowser->clear();
	ui.lineEdit->clear();
	ui.comboBox->setCurrentIndex(0);
}

void LoggerMainWindow::slotSocketDisconected()
{
	VERIFY(disconnect((QTcpSocket*)sender(), SIGNAL(readyRead()), this, SLOT(slotReadyRead())));
	VERIFY(disconnect(this, SIGNAL(messageAppendedToList()), this, SLOT(slotTryToShowNewMessage())));
	ui.textBrowser->append("disconnected");

	qApp->quit();
}

void LoggerMainWindow::slotApplyTypeFilter(const QString& type)
{
	if (m_currentTypeFilter != typeFromString(type))
	{
		m_currentTypeFilter = typeFromString(type);
		redrawList();
	}
}

void LoggerMainWindow::slotTryToShowNewMessage()
{
	if ((m_currentTypeFilter == NoType || m_incomingMessages.last().type == m_currentTypeFilter) &&
		(m_currentStringFilter.isEmpty() || m_incomingMessages.last().toString().contains(m_currentStringFilter, Qt::CaseInsensitive)))
	{
		ui.textBrowser->setTextColor(identifyMessageColor(m_incomingMessages.last().type));
		ui.textBrowser->append(m_incomingMessages.last().toString());
	}
}

void LoggerMainWindow::redrawList()
{
	ui.textBrowser->clear();
	for (const auto& currentMessage : m_incomingMessages)
	{
		if ((m_currentTypeFilter == NoType || currentMessage.type == m_currentTypeFilter) && 
			(m_currentStringFilter.isEmpty() || currentMessage.toString().contains(m_currentStringFilter, Qt::CaseInsensitive)))
		{
			ui.textBrowser->setTextColor(identifyMessageColor(currentMessage.type));
			ui.textBrowser->append(currentMessage.toString());
		}
	}	
}

QColor LoggerMainWindow::identifyMessageColor(MessageType type)
{
	switch (type)
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

		m_blockSize = 0;
	}
}
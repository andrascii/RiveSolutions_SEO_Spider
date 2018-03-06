#include "log_message_receiver.h"

namespace SeoSpiderService
{

LogMessageReceiver::LogMessageReceiver(QObject* parent)
	: QObject(parent)
	, m_socket(new QLocalSocket(this))
	, m_message{}
	, m_connectionTimerId{}
{
	assert(m_message.messageSize == 0);
	VERIFY(connect(m_socket, &QLocalSocket::disconnected, this, &LogMessageReceiver::onConnectionClosed));
	VERIFY(connect(m_socket, &QLocalSocket::readyRead, this, &LogMessageReceiver::readMessage));

	m_socket->connectToServer("LogServerDataAccumulator");
	m_socket->waitForConnected();

	qRegisterMetaType<Message>("Message");

	if (m_socket->state() != QLocalSocket::ConnectedState)
	{
		m_connectionTimerId = startTimer(1000);

		Q_ASSERT(m_connectionTimerId);
	}
}

void LogMessageReceiver::timerEvent(QTimerEvent*)
{
	m_socket->connectToServer("LogServerDataAccumulator");
	m_socket->waitForConnected();

	if (m_socket->state() == QLocalSocket::ConnectedState)
	{
		killTimer(m_connectionTimerId);
	}
}

void LogMessageReceiver::readMessage()
{
	QDataStream stream(m_socket);
	stream.setVersion(QDataStream::Qt_4_0);

	static bool messageSizeReceived = false;
	constexpr uint headerSize = static_cast<uint>(sizeof(qint64) * 2);

	for (;;)
	{
		if (m_message.messageSize == 0)
		{
			if (messageSizeReceived)
			{
				qDebug() << "Error: Message is received;";
				assert(false);
			}

			if (m_socket->bytesAvailable() < headerSize)
			{
				break;
			}

			char headerBuffer[headerSize];
			char* headerBufferPtr = headerBuffer;
			qint64* messageSizePtr = (qint64*)headerBuffer;
			qint64* severityPtr = (qint64*)(headerBuffer + headerSize / 2);
			uint headerSizeLValue = headerSize;

			stream.readRawData(headerBufferPtr, headerSizeLValue);

			m_message.messageSize = *messageSizePtr;
			m_message.severityLevel = static_cast<SeoSpiderServiceApi::SeverityLevel>(*severityPtr);

			messageSizeReceived = true;
			qDebug() << "Message size received " << m_message.messageSize;
			if (m_message.messageSize > 1000 || m_message.messageSize < 1)
			{
				qDebug() << "m_message.messageSize > 1000 || m_message.messageSize < 1";
				assert(false);
			}
		}

		//qDebug() << "Message before reading " << m_message.messageSize << " available " << m_socket->bytesAvailable();

		if (!messageSizeReceived)
		{
			qDebug() << "Error: Message is not received;";
			assert(false);
		}
		
		if (m_message.messageSize > 1000 || m_message.messageSize < 1)
		{
			qDebug() << "m_message.messageSize > 1000 || m_message.messageSize < 1 (1)";
			assert(false);
		}
		
		if (m_socket->bytesAvailable() < m_message.messageSize || stream.atEnd())
		{
			continue;
		}

		//qDebug() << "Message size reading " << m_message.messageSize;

		if (m_message.messageSize > 1000 || m_message.messageSize < 1)
		{
			qDebug() << "m_message.messageSize > 1000 || m_message.messageSize < 1 (2)";
			assert(false);
		}
		char* buffer = new char[m_message.messageSize];

		uint length = static_cast<uint>(m_message.messageSize);
		stream.readRawData(buffer, length);
		m_message.message = QString::fromUtf8(buffer, static_cast<int>(m_message.messageSize));

		delete[] buffer;

		emit messageReceived(m_message);

		m_message = Message{};
		if (m_message.messageSize != 0)
		{
			qDebug() << "m_message.messageSize == 0";
			assert(false);
		}
		messageSizeReceived = false;
	}
}

void LogMessageReceiver::onConnectionClosed()
{
	if (m_socket)
	{
		m_socket->deleteLater();
		m_socket = nullptr;
	}
}

}
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


	class LoggerDataStream 
	{
		
	public:
		LoggerDataStream(MessageType type, QString func, QString file, int line) :
			m_messageType(type),
			m_fucntionName(func),
			m_fileName(file),
			m_lineNumber(line) 
		{
			m_textStream.setString(&m_message, QIODevice::WriteOnly);
		};

		LoggerDataStream(LoggerDataStream&);
		~LoggerDataStream();

		template<typename T>
		LoggerDataStream& operator<<(T str)
		{
			this->m_textStream << str << ' ';
			return *this;
		}

	private:
		MessageType m_messageType;
		QString m_fucntionName;
		QString m_fileName;
		int m_lineNumber;
		QString m_message;
		QTextStream m_textStream;
	};

public:
	static LoggerConnectionServiceApi* instance();

	void addPackToStream(QTextStream& messageStream)
	{
		return;
	}

	template<typename T>
	void addPackToStream(QTextStream& messageStream, const T& value)
	{
		messageStream << value;
	}

	template <typename First, typename... Rest> 
	void addPackToStream(QTextStream& messageStream, const First& first, const Rest&... rest)
	{
		messageStream << first << ' ';
		addPackToStream(messageStream, rest...);
	}

	LoggerConnectionServiceApi::LoggerDataStream LoggerConnectionServiceApi::log(MessageType type, QString func, QString file, int line);

	template<typename ... StringPack>
	void log(MessageType type, QString func, QString file, int line, const StringPack&... textPack)
	{
		std::lock_guard<std::mutex> locker(m_mutex);

		if (!m_socket->isOpen())
		{
			return;
		}

		QByteArray block;		
		QString message;
		QTextStream messageStream(&message, QIODevice::WriteOnly);
		QDataStream out(&block, QIODevice::WriteOnly);
		QString date(QDateTime::currentDateTime().toString());

		out.setVersion(QDataStream::Qt_4_0);

		out << static_cast<quint16>(0);


		messageStream 
			<< type << '|' 
			<< date << '|'
			<< func << '|' 
			<< file << '|'
			<< line << '|';
		addPackToStream(messageStream, textPack...);

		out << message;
		out.device()->seek(0);

		out << static_cast<quint16>(block.size() - sizeof(quint16));

		m_socket->write(block);
		m_socket->waitForBytesWritten();
	}

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
#pragma once
#include <QProcess>
#include <QLocalSocket>

namespace QuickieWebBot 
{
	class Logger : public QObject
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

	public slots:
		void sendMessage();
		void connected();
		void showError();

	public:

		static Logger* instance();
		void log(MessageType type, QString tag, QString text, QString func) const noexcept;
		void init();
	private:
		Logger(QObject* parent = 0);


	private:
		QProcess* m_loggerProc;
		QTcpSocket* socket;
		QTimer connectionTimer;
		int blockSize;
		bool m_connected;
	};
}
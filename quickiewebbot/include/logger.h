#pragma once
#include <QProcess>
#include <QLocalSocket>

namespace QuickieWebBot
{
	class Logger
	{
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
		void log(MessageType type, QString tag, QString text) const noexcept;
		void init();
	private:
		Logger();


	private:
		QProcess* m_loggerProc;
		QLocalSocket* socket;
		QTimer connectionTimer;
		int blockSize;
		bool m_connected;
	};
}
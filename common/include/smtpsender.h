#pragma once

namespace Common
{

class SmtpSettings : public QObject
{
	Q_OBJECT	
public:
	Q_PROPERTY(QString emailSender READ emailSender WRITE setEmailSender)
	Q_PROPERTY(QString emailRecipients READ emailRecipients WRITE setEmailRecipients)
	Q_PROPERTY(QString emailSmtpHost READ emailSmtpHost WRITE setEmailSmtpHost)
	Q_PROPERTY(int emailSmtpPort READ emailSmtpPort WRITE setEmailSmtpPort)
	Q_PROPERTY(QString emailUsername READ emailUsername WRITE setEmailUsername)
	Q_PROPERTY(QString emailPassword READ emailPassword WRITE setEmailPassword)
	Q_PROPERTY(bool emailUseAuthentication READ emailUseAuthentication WRITE setEmailUseAuthentication)
	Q_PROPERTY(bool emailUseSsl READ emailUseSsl WRITE setEmailUseSsl)
	Q_PROPERTY(int emailTimeout READ emailTimeout WRITE setEmailTimeout)

public:
	QString emailSender() const;
	Q_SLOT void setEmailSender(QString emailSender);

	QString emailRecipients() const;
	Q_SLOT void setEmailRecipients(QString emailRecipients);

	QString emailSmtpHost() const;
	Q_SLOT void setEmailSmtpHost(QString emailSmtpHost);

	int emailSmtpPort() const;
	Q_SLOT void setEmailSmtpPort(int emailSmtpPort);

	QString emailUsername() const;
	Q_SLOT void setEmailUsername(QString emailUsername);

	QString emailPassword() const;
	Q_SLOT void setEmailPassword(QString emailPassword);

	bool emailUseAuthentication() const;
	Q_SLOT void setEmailUseAuthentication(bool emailUseAuthenticetion);

	bool emailUseSsl() const;
	Q_SLOT void setEmailUseSsl(bool emailUseSsl);

	int emailTimeout() const;
	Q_SLOT void setEmailTimeout(int emailTimeout);

private:
	QString m_sender;
	QString m_recipients;
	QString m_serverHost;
	int m_serverPort;
	QString m_username;
	QString m_password;
	int m_timeout;
	bool m_useAutentication;
	bool m_useSsl;

public:
	SmtpSettings();
	SmtpSettings(const SmtpSettings& other);
	~SmtpSettings();
};

struct SmtpMessage
{
	SmtpSettings settings;
	QString subject;
	QString message;
	QStringList attachedFilePaths;

	SmtpMessage(const SmtpSettings& settings, const QString& subject, const QString& message, const QStringList& attachedFilePaths = QStringList())
		: settings(settings)
		, subject(subject)
		, message(message)
		, attachedFilePaths(attachedFilePaths)
	{}
};

class SmtpSender : private QObject
{
	Q_OBJECT
public:
	enum
	{
		resultSuccess,
		resultFailed,
		resultConnectionFailed,
		resultEncryptionFailed,
		resultAuthenticationFailed,
		resultSenderRejectedAddress,
		resultRecipientRejectedAddress,
		resultInvalidFrom,
		resultInvalidTo,
		resultTimeout,
		resultUnknown
	} Result;

	static int send(const SmtpMessage& message, QObject* listener = NULL, const char* onSentSlot = NULL);

	static void init();
	static void term();

	Q_SIGNAL void sendingFinished(int id, int result, const QByteArray& log);

private:
	SmtpSender();
	static SmtpSender* instance();

private:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	int m_timerId;
};

}
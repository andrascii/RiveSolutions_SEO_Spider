#pragma once

#include "smtpsender.h"
#include "qxtsmtp.h"

namespace Common
{

class SmtpSendingResult : public QObject
{
	Q_OBJECT
public:
	SmtpSendingResult(const SmtpMessage& message);
	~SmtpSendingResult();

	bool isFinished() const;

	void abortByTimeout();

	int instanceId() const;

	void processState();

	QByteArray log() const;

signals:
	void sendingFinished(int id, int result, const QByteArray& log);

private slots:
	void on_smtp_connected();
	void on_smtp_connectionFailed();
	void on_smtp_connectionFailed( const QByteArray& msg );
	void on_smtp_encrypted();
	void on_smtp_encryptionFailed();
	void on_smtp_encryptionFailed( const QByteArray& msg );
	void on_smtp_authenticated();
	void on_smtp_authenticationFailed();
	void on_smtp_authenticationFailed( const QByteArray& msg );

	void on_smtp_senderRejected(int mailID, const QString& address );
	void on_smtp_senderRejected(int mailID, const QString& address, const QByteArray& msg );
	void on_smtp_recipientRejected(int mailID, const QString& address );
	void on_smtp_recipientRejected(int mailID, const QString& address, const QByteArray& msg );
	void on_smtp_mailFailed(int mailID, int errorCode);
	void on_smtp_mailFailed(int mailID, int errorCode, const QByteArray& msg);
	void on_smtp_mailSent(int mailID);

	void on_smtp_finished();
	void on_smtp_disconnected();

private:
	void addToLog(const QByteArray& event);
	void finish(int result);

	static bool isValidEmailAddress(const QString& from);

private:
	QByteArray m_log;
	bool m_finished;
	int m_result;
	QxtSmtp* m_smtp;
	QTime m_timeoutTime;

	int m_instanceId;
};

}

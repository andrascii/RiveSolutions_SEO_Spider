#include "smtp_sending_result.h"
#include "smtp_sender.h"

namespace Common
{

QStringList splitRecipients(const QString& recipients)
{
	QStringList result = recipients.split(QLatin1Char(';'), QString::SkipEmptyParts);

	foreach(QString item, result)
	{
		item = item.trimmed();
	}

	return result;
};

int resultInstanceId = 0;

SmtpSendingResult::SmtpSendingResult(const SmtpMessage& message, const QString& messageId)
	: m_finished(false)
	, m_result(SmtpSender::resultUnknown)
	, m_timeoutTime(QTime::currentTime().addSecs(message.settings.emailTimeout()))
	, m_instanceId(resultInstanceId++)
	, m_messageId(messageId)
{
	const QStringList recipients = splitRecipients(message.settings.emailRecipients());

	foreach(const QString& recipient, recipients)
	{
		if (!isValidEmailAddress(recipient))
		{
			addToLog("To is not a valid e-mail address");
			finish(SmtpSender::resultInvalidTo);
			return;
		}
	}

	if (!isValidEmailAddress(message.settings.emailSender()))
	{
		addToLog("From is not a valid e-mail address");
		finish(SmtpSender::resultInvalidFrom);
		return;
	}

	m_smtp = new QxtSmtp(this);
	m_smtp->setObjectName(QLatin1String("smtp"));
	QMetaObject::connectSlotsByName(this);

	QxtMailMessage m;

	foreach(QString item, recipients)
	{
		m.addRecipient(item);
	}

	m.setSubject(message.subject);
	m.setBody(message.message);
	m.setSender(message.settings.emailSender());
	if (!message.attachedFilePaths.isEmpty())
	{
		foreach (QString attachedFilePath, message.attachedFilePaths)
		{
			if (!QFileInfo(attachedFilePath).exists())
			{
				//WARNLOG << "File not found" << attachedFilePath;
				continue;
			}
			QFile attachedFile(attachedFilePath);
			attachedFile.open(QIODevice::ReadOnly);
			QString fileExtension = QFileInfo(attachedFilePath).suffix();
			QxtMailAttachment attachment(attachedFile.readAll());
			attachedFile.close();
			m.addAttachment(attachedFilePath, attachment);
		}		
	}

	const QTextCodec* const latin1 = QTextCodec::codecForName("latin1");
	bool bodyIsAscii = latin1->canEncode(message.message);

	if (!bodyIsAscii)
	{
		m.setExtraHeader(QLatin1String("Content-Type"), QLatin1String("text/plain; charset=utf-8"));
		m.setExtraHeader(QLatin1String("Content-Transfer-Encoding"), QLatin1String("base64"));
	}

	if (message.settings.emailUseAuthentication())
	{
		m_smtp->setUsername(message.settings.emailUsername().toLatin1());
		m_smtp->setPassword(message.settings.emailPassword().toLatin1());
	}

	if (message.settings.emailUseSsl())
	{
#ifndef QT_NO_OPENSSL
		m_smtp->connectToSecureHost(message.settings.emailSmtpHost(),
			message.settings.emailSmtpPort());
#else
		m_smtp->connectToHost(message.settings.emailSmtpHost(),
			message.settings.emailSmtpPort());
#endif
	} 
	else
	{
		m_smtp->connectToHost(message.settings.emailSmtpHost(),
			message.settings.emailSmtpPort());
	}

	m_smtp->send(m);
}

SmtpSendingResult::~SmtpSendingResult()
{
}

void SmtpSendingResult::on_smtp_connected()
{
	addToLog("Connected.");
}

void SmtpSendingResult::on_smtp_connectionFailed()
{
	addToLog("Connection failed.");
	finish(SmtpSender::resultConnectionFailed);
}

void SmtpSendingResult::on_smtp_connectionFailed(const QByteArray & msg)
{
	Q_UNUSED(msg);
}

void SmtpSendingResult::on_smtp_encrypted()
{
	addToLog("Encrypted mail session.");
}

void SmtpSendingResult::on_smtp_encryptionFailed()
{
	addToLog("Encryption failed.");
	finish(SmtpSender::resultEncryptionFailed);
}

void SmtpSendingResult::on_smtp_encryptionFailed( const QByteArray & msg )
{
	Q_UNUSED(msg);
}

void SmtpSendingResult::on_smtp_authenticated()
{
	addToLog("Authenticated successfully.");
}

void SmtpSendingResult::on_smtp_authenticationFailed()
{
	addToLog("Authentication failed.");
}

void SmtpSendingResult::on_smtp_authenticationFailed(const QByteArray& msg)
{
	addToLog(("Authentication failed: ") + msg);
	finish(SmtpSender::resultAuthenticationFailed);
}

void SmtpSendingResult::on_smtp_senderRejected(int mailID, const QString& address)
{
	Q_UNUSED(mailID);

	addToLog(("Sender rejected address: ") + address.toUtf8());
	finish(SmtpSender::resultSenderRejectedAddress);
}

void SmtpSendingResult::on_smtp_senderRejected(int mailID, const QString& address, const QByteArray& msg)
{
	Q_UNUSED(mailID);
	Q_UNUSED(address);
	Q_UNUSED(msg);
}

void SmtpSendingResult::on_smtp_recipientRejected(int mailID, const QString& address)
{
	Q_UNUSED(mailID);

	addToLog(("Recipient address rejected: ") + address.toUtf8());
	finish(SmtpSender::resultRecipientRejectedAddress);
}

void SmtpSendingResult::on_smtp_recipientRejected(int mailID, const QString& address, const QByteArray& msg)
{
	Q_UNUSED(mailID);
	Q_UNUSED(address);
	Q_UNUSED(msg);
}

void SmtpSendingResult::on_smtp_mailFailed(int mailID, int errorCode)
{
	Q_UNUSED(mailID);

	addToLog(QString(QLatin1String("Mail failed with error: %0.")).arg(errorCode).toUtf8());
	finish(SmtpSender::resultFailed);
}

void SmtpSendingResult::on_smtp_mailFailed(int mailID, int errorCode, const QByteArray& msg)
{
	Q_UNUSED(mailID);
	Q_UNUSED(errorCode);
	Q_UNUSED(msg);
}

void SmtpSendingResult::on_smtp_mailSent(int mailID)
{
	Q_UNUSED(mailID);

	addToLog("Message sent successfully.");
	m_smtp->disconnectFromHost();
	finish(SmtpSender::resultSuccess);
}

void SmtpSendingResult::on_smtp_finished()
{
}

void SmtpSendingResult::on_smtp_disconnected()
{
}

bool SmtpSendingResult::isFinished() const
{
	return m_finished;
}

void SmtpSendingResult::addToLog(const QByteArray& event)
{
	if (!m_log.isEmpty())
	{
		m_log += "\r\n";
	}

	m_log += event;
}

QByteArray SmtpSendingResult::log() const
{
	return m_log;
}

int SmtpSendingResult::instanceId() const
{
	return m_instanceId;
}

void SmtpSendingResult::finish(int result)
{
	m_finished = true;
	m_result = result;
}

bool SmtpSendingResult::isValidEmailAddress(const QString& from)
{
	static const QRegExp exp(QLatin1String("^([a-zA-Z0-9_\\-\\.]+)@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.)|(([a-zA-Z0-9\\-]+\\.)+))([a-zA-Z]{2,5}|[0-9]{1,5})(\\]?)$"));

	return exp.exactMatch(from);
}

void SmtpSendingResult::processState()
{
	if (m_timeoutTime <= QTime::currentTime())
	{
		addToLog("Aborted by timeout.");
		finish(SmtpSender::resultTimeout);
	}

	if (isFinished())
	{
		Q_EMIT sendingFinished(m_messageId, m_result, m_log);
		return;
	}
}

}
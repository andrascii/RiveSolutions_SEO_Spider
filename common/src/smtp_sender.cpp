#include "stdafx.h"
#include "smtp_sender.h"
#include "smtp_sending_result.h"

namespace Common
{

SmtpSettings::SmtpSettings()
	: m_timeout(30)
	, m_useAutentication(false)
	, m_useSsl(false)
	, m_serverPort(25)
{}

SmtpSettings::SmtpSettings(const SmtpSettings& other)
{
	m_sender = other.m_sender;
	m_recipients = other.m_recipients;
	m_serverHost = other.m_serverHost;
	m_serverPort = other.m_serverPort;
	m_username = other.m_username;
	m_password = other.m_password;
	m_timeout = other.m_timeout;
	m_useAutentication = other.m_useAutentication;
	m_useSsl = other.m_useSsl;
}


QString SmtpSettings::emailSender() const
{
	return m_sender;
}

void SmtpSettings::setEmailSender(QString emailSender)
{
	m_sender = emailSender;
}

QString SmtpSettings::emailRecipients() const
{
	return m_recipients;
}

void SmtpSettings::setEmailRecipients(QString emailRecipients)
{
	m_recipients = emailRecipients;
}

QString SmtpSettings::emailSmtpHost() const
{
	return m_serverHost;
}

void SmtpSettings::setEmailSmtpHost(QString emailSmtpHost)
{
	m_serverHost = emailSmtpHost;
}

int SmtpSettings::emailSmtpPort() const
{
	return m_serverPort;
}

void SmtpSettings::setEmailSmtpPort(int emailSmtpPort)
{
	m_serverPort = emailSmtpPort;
}

QString SmtpSettings::emailUsername() const
{
	return m_username;
}

void SmtpSettings::setEmailUsername(QString emailUsername)
{
	m_username = emailUsername;
}

QString SmtpSettings::emailPassword() const
{
	return m_password;
}

void SmtpSettings::setEmailPassword(QString emailPassword)
{
	m_password = emailPassword;
}

bool SmtpSettings::emailUseAuthentication() const
{
	return m_useAutentication;
}

void SmtpSettings::setEmailUseAuthentication(bool emailUseAuthenticetion)
{
	m_useAutentication = emailUseAuthenticetion;
}

bool SmtpSettings::emailUseSsl() const
{
	return m_useSsl;
}

void SmtpSettings::setEmailUseSsl(bool emailUseSsl)
{
	m_useSsl = emailUseSsl;
}

int SmtpSettings::emailTimeout() const
{
	return m_timeout;
}

void SmtpSettings::setEmailTimeout(int emailTimeout)
{
	m_timeout = emailTimeout;
}

SmtpSettings::~SmtpSettings()
{
}

QVector<SmtpSendingResult*> g_sendingResults;

int SmtpSender::send(const SmtpMessage& message, const QString& messageId, QObject* listener /*= NULL*/, const char* onSentSlot/*= NULL*/)
{
	SmtpSendingResult* task = new SmtpSendingResult(message, messageId);

	assert((listener == NULL && onSentSlot == NULL) || (listener != NULL && onSentSlot != NULL));

	if (NULL != listener && NULL != onSentSlot)
	{
		/*VERIFY(*/QObject::connect(task, SIGNAL(sendingFinished(const QString&, int, const QByteArray&)), listener, onSentSlot)/*)*/;
	}

	if (g_sendingResults.isEmpty())
	{
		const int processPeriod = 500;
		instance()->m_timerId = instance()->startTimer(processPeriod);
	}

	if (!g_sendingResults.contains(task))
	{
		task->setParent(instance());
		g_sendingResults.append(task);
	}

	return task->instanceId();
}

std::unique_ptr<SmtpSender> g_instance;

void SmtpSender::init()
{
	g_instance.reset(new SmtpSender());
}

void SmtpSender::term()
{
	g_instance.reset();
}

SmtpSender* SmtpSender::instance()
{
	return g_instance.get();
}

SmtpSender::SmtpSender()
	: m_timerId(-1)
{
}

void SmtpSender::timerEvent(QTimerEvent* event)
{
	Q_UNUSED(event);
	assert(event->timerId() == m_timerId);

	QVector<SmtpSendingResult*> survivors;

	foreach(SmtpSendingResult* item, g_sendingResults)
	{
		item->processState();

		if (item->isFinished())
		{
			delete item;
			continue;
		} 
		survivors.append(item);
	}

	g_sendingResults = survivors;

	if (g_sendingResults.isEmpty())
	{
		killTimer(m_timerId);
		m_timerId = -1;
	}
}

}
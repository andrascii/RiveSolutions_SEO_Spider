#include "internet_connection_inspector.h"

#pragma comment(lib, "iphlpapi.lib")

namespace
{

using namespace SeoSpider;

InternetConnectionInspector* s_internetConnectionInspector = nullptr;

constexpr int s_pingInterval = 1000;

}

namespace SeoSpider
{

InternetConnectionInspector* InternetConnectionInspector::instance()
{
	return s_internetConnectionInspector;
}

void InternetConnectionInspector::init()
{
	if (!s_internetConnectionInspector)
	{
		QThread* thread = new QThread;
		s_internetConnectionInspector = new InternetConnectionInspector;

		s_internetConnectionInspector->moveToThread(thread);

		VERIFY(QMetaObject::invokeMethod(s_internetConnectionInspector, "start", Qt::QueuedConnection));
	}
}

void InternetConnectionInspector::term()
{
	QThread* thread = s_internetConnectionInspector->thread();

	s_internetConnectionInspector->deleteLater();

	qApp->processEvents();

	thread->quit();
	thread->wait();

	s_internetConnectionInspector = nullptr;
}

bool InternetConnectionInspector::internetAvailable() const noexcept
{
	return m_internetAvailable;
}

void InternetConnectionInspector::start()
{
	m_timerId = startTimer(s_pingInterval);
}

void InternetConnectionInspector::stop()
{
	killTimer(m_timerId);
	m_timerId = 0;
}

void InternetConnectionInspector::timerEvent(QTimerEvent*)
{
	const bool internetAvailable = doPing();

	if (internetAvailable != m_internetAvailable)
	{
		m_internetAvailable = internetAvailable;
		emit statusChanged();
	}
}

InternetConnectionInspector::InternetConnectionInspector(QObject* parent)
	: QObject(parent)
	, m_timerId(0)
	, m_sendData("Send Data")
	, m_replyBufferSize(sizeof(ICMP_ECHO_REPLY) + m_sendData.size() + 8)
	, m_replyBuffer(new char[m_replyBufferSize])
	, m_internetAvailable(false)
{
	m_hostList.append(QStringLiteral("4.2.2.2"));
	m_hostList.append(QStringLiteral("8.8.8.8"));
	m_hostList.append(QStringLiteral("8.8.4.4"));
	m_hostList.append(QStringLiteral("4.2.2.1"));
}

InternetConnectionInspector::~InternetConnectionInspector()
{
	stop();
}

bool InternetConnectionInspector::doPing() const noexcept
{
	foreach(const QString& host, m_hostList)
	{
		HANDLE hIcmpFile;
		unsigned long ipaddr = inet_addr(host.toStdString().c_str());

		DWORD dwRetVal = 0;

		hIcmpFile = IcmpCreateFile();

		dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, (LPVOID*)m_sendData.data(), static_cast<WORD>(m_sendData.size()),
			NULL, m_replyBuffer.get(), static_cast<DWORD>(m_replyBufferSize), s_pingInterval);

		PICMP_ECHO_REPLY pIcmpEchoReply = reinterpret_cast<PICMP_ECHO_REPLY>(m_replyBuffer.get());
		IcmpCloseHandle(hIcmpFile);

		if (dwRetVal != 0 && pIcmpEchoReply->Status == IP_SUCCESS)
		{
			return true;
		}
	}

	return false;
}

}
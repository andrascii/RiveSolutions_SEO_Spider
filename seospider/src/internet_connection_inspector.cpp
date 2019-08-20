#include "stdafx.h"

#include "internet_connection_inspector.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "iphlpapi.lib")
#endif

namespace
{

/*
#ifdef Q_OS_MACOS


#define PACKETSIZE  64
struct packet
{
    struct icmphdr hdr;
    char msg[PACKETSIZE-sizeof(struct icmphdr)];
};

int pid=-1;
struct protoent *proto=NULL;
int cnt=1;
 
 */

/*--------------------------------------------------------------------*/
/*--- checksum - standard 1s complement checksum                   ---*/
/*--------------------------------------------------------------------*/
    
/*
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum=0;
    unsigned short result;
    
    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}
    
/*--------------------------------------------------------------------*/
/*--- ping - Create message and send it.                           ---*/
/*    return 0 is ping Ok, return 1 is ping not OK.                ---*/
/*--------------------------------------------------------------------
int ping(char *adress)
{
    const int val=255;
    int i, sd;
    struct packet pckt;
    struct sockaddr_in r_addr;
    int loop;
    struct hostent *hname;
    struct sockaddr_in addr_ping,*addr;
    
    pid = getpid();
    proto = getprotobyname("ICMP");
    hname = gethostbyname(adress);
    bzero(&addr_ping, sizeof(addr_ping));
    addr_ping.sin_family = hname->h_addrtype;
    addr_ping.sin_port = 0;
    addr_ping.sin_addr.s_addr = *(long*)hname->h_addr;
    
    addr = &addr_ping;
    
    sd = socket(PF_INET, SOCK_RAW, proto->p_proto);
    if ( sd < 0 )
    {
        perror("socket");
        return 1;
    }
    if ( setsockopt(sd, SOL_IP, IP_TTL, &val, sizeof(val)) != 0)
    {
        perror("Set TTL option");
        return 1;
    }
    if ( fcntl(sd, F_SETFL, O_NONBLOCK) != 0 )
    {
        perror("Request nonblocking I/O");
        return 1;
    }
    
    for (loop=0;loop < 10; loop++)
    {
        
        int len=sizeof(r_addr);
        
        if ( recvfrom(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)&r_addr, &len) > 0 )
        {
            return 0;
        }
        
        bzero(&pckt, sizeof(pckt));
        pckt.hdr.type = ICMP_ECHO;
        pckt.hdr.un.echo.id = pid;
        for ( i = 0; i < sizeof(pckt.msg)-1; i++ )
            pckt.msg[i] = i+'0';
        pckt.msg[i] = 0;
        pckt.hdr.un.echo.sequence = cnt++;
        pckt.hdr.checksum = checksum(&pckt, sizeof(pckt));
        if ( sendto(sd, &pckt, sizeof(pckt), 0, (struct sockaddr*)addr, sizeof(*addr)) <= 0 )
            perror("sendto");
        
        usleep(300000);
        
    }
    
    return 1;
}

#endif
 */

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
		thread->start();

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
		emit statusChanged(m_internetAvailable);
	}
}

InternetConnectionInspector::InternetConnectionInspector(QObject* parent)
	: QObject(parent)
	, m_sendData("Send Data")
    , m_timerId(0)
	//, m_replyBufferSize(sizeof(ICMP_ECHO_REPLY) + m_sendData.size() + 8)
	//, m_replyBuffer(new char[m_replyBufferSize])
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
    /*
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
     */
    
    return true;
}

}

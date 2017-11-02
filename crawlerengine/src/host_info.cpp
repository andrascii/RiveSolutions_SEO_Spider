#include "host_info.h"

#if defined Q_OS_WIN

//
// May be these libs need to link using CMake...
//

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "windowsapp.lib")

#endif

namespace CrawlerEngine
{

HostInfo::HostInfo(const QByteArray& hostname)
	: m_valid(true)
	, m_error("No error")
{
	hostent* remoteHost = ::gethostbyname(hostname.constData());

	if (!remoteHost)
	{
		m_valid = false;
		determineError();
		return;
	}

	m_officialHostname = remoteHost->h_name;

	setAliases(remoteHost);
	setAddressFamily(remoteHost);
	setIpAddresses(remoteHost);
}

bool HostInfo::isValid() const noexcept
{
	return m_valid;
}

const QByteArray& HostInfo::error() const
{
	return m_error;
}

const QByteArray& HostInfo::officialHostname() const noexcept
{
	return m_officialHostname;
}

const QList<QByteArray>& HostInfo::hostnameAliases() const noexcept
{
	return m_aliases;
}

QList<in_addr> HostInfo::addressesIPv4() const
{
	return m_ipv4Addresses.values();
}

QList<in6_addr> HostInfo::addressesIPv6() const
{
	return m_ipv6Addresses.values();
}

QList<QByteArray> HostInfo::stringAddressesIPv4() const
{
	return m_ipv4Addresses.keys();
}

QList<QByteArray> HostInfo::stringAddressesIPv6() const
{
	return m_ipv6Addresses.keys();
}

AddressFamily HostInfo::addressFamily() const noexcept
{
	return m_addressFamily;
}

void HostInfo::determineError()
{
#ifdef Q_OS_WIN
	
	const DWORD error = ::WSAGetLastError();

	switch (error)
	{
		case WSANOTINITIALISED:
		{
			m_error = "A successful WSAStartup call must occur before using this function.";
			break;
		}
		case WSAENETDOWN:
		{
			m_error = "The network subsystem has failed.";
			break;
		}
		case WSAHOST_NOT_FOUND:
		{
			m_error = "Authoritative answer host not found.";
			break;
		}
		case WSATRY_AGAIN:
		{
			m_error = "Nonauthoritative host not found, or server failure.";
			break;
		}
		case WSANO_RECOVERY:
		{
			m_error = "A nonrecoverable error occurred.";
			break;
		}
		case WSANO_DATA:
		{
			m_error =
				"The requested name is valid, but no data of the requested type was found. "
				"This error is also returned if the name parameter contains a string " 
				"representation of an IPv6 address or an illegal IPv4 address.\n\n"
				"This error should not be interpreted to mean that the name parameter contains a name string "
				"that has been validated for a particular protocol(an IP hostname, for example). "
				"Since Winsock supports multiple name service providers, a name may potentially be valid for one "
				"provider and not accepted by another provider.";

				break;
		}
		case WSAEINPROGRESS:
		{
			m_error = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
			break;
		}
		case WSAEFAULT:
		{
			m_error = "The name parameter is not a valid part of the user address space.";
			break;
		}
		case WSAEINTR:
		{
			m_error = "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";
			break;
		}
	}

#else

	//
	// For UNIX-like operating systems check h_errno variable
	//

	m_error = "Invalid hostname!";

#endif

}

void HostInfo::setAddressFamily(hostent* remoteHost)
{
	switch (remoteHost->h_addrtype)
	{
		case AF_INET:
		{
			m_addressFamily = AddressFamily::IPv4;
			break;
		}
		case AF_INET6:
		{
			m_addressFamily = AddressFamily::IPv6;
			break;
		}
		case AF_NETBIOS:
		{
			m_addressFamily = AddressFamily::NetBios;
			break;
		}
		default:
		{
			m_addressFamily = AddressFamily::UnknownAddressFamily;
			break;
		}
	}
}

void HostInfo::setAliases(hostent* remoteHost)
{
	for (char** alias = remoteHost->h_aliases; *alias != nullptr; ++alias)
	{
		m_aliases.push_back(*alias);
	}
}

void HostInfo::setIpAddresses(hostent* remoteHost)
{
	if (m_addressFamily == AddressFamily::IPv4)
	{
		int i = 0;

		while (remoteHost->h_addr_list[i] != nullptr)
		{
			in_addr addr;
			addr.s_addr = *(unsigned long*)remoteHost->h_addr_list[i++];

			CHAR ipStringRepresentation[16] = { 0 };
			::RtlIpv4AddressToStringA(&addr, ipStringRepresentation);
			
			m_ipv4Addresses[ipStringRepresentation] = addr;
		}
	}

	if (m_addressFamily == AddressFamily::IPv6)
	{
		int i = 0;

		while (remoteHost->h_addr_list[i] != nullptr)
		{
			in6_addr addr;

			std::uint16_t* wordPointer = (std::uint16_t*)remoteHost->h_addr_list[i++];
			std::memcpy(addr.s6_words, wordPointer, sizeof(in6_addr) / sizeof(std::uint16_t));

			CHAR ipStringRepresentation[46] = {0};

			::RtlIpv6AddressToStringA(&addr, ipStringRepresentation);
			m_ipv6Addresses[ipStringRepresentation] = addr;
		}
	}
}

}
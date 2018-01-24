#pragma once

namespace CrawlerEngine
{

enum class AddressFamily
{
	IPv4,
	IPv6,
	NetBios,
	UnknownAddressFamily
};

class HostInfo
{
public:
	HostInfo(const QByteArray& webpage);

	bool isValid() const noexcept;
	const QByteArray& error() const;
	const QByteArray& officialHostname() const noexcept;
	const QList<QByteArray>& hostnameAliases() const noexcept;
	QList<in_addr> addressesIPv4() const;
	QList<in6_addr> addressesIPv6() const;
	QList<QByteArray> stringAddressesIPv4() const;
	QList<QByteArray> stringAddressesIPv6() const;
	AddressFamily addressFamily() const noexcept;

protected:
	//
	// Creates fake valid host info pair [localhost; 127.0.0.1]
	//
	HostInfo();

private:
	void determineError();
	void setAddressFamily(hostent* remoteHost);
	void setAliases(hostent* remoteHost);
	void setIpAddresses(hostent* remoteHost);

private:
	QByteArray m_officialHostname;
	QList<QByteArray> m_aliases;
	QMap<QByteArray, in_addr> m_ipv4Addresses;
	QMap<QByteArray, in6_addr> m_ipv6Addresses;
	bool m_valid;
	QByteArray m_error;
	AddressFamily m_addressFamily;
};

}
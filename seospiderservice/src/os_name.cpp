#include "os_name.h"

#ifdef Q_OS_WIN

#include <QSettings>
#include <windows.h>

#include "get_version_windows.h"
#include "file_version.h"

namespace SeoSpiderService
{

#define VER_SUITE_WH_SERVER 0x00008000

QString getWindowsXp32(const OSVERSIONINFOEX& osVerInfo)
{
	if (osVerInfo.wSuiteMask & VER_SUITE_PERSONAL)
	{
		return QString("Windows XP Home Edition");
	}
	else
	{
		return QString("Windows XP Professional");
	}
}

QString getWindowsServer2003Type(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	QString serverType;

	if (sysInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64)
	{
		if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER )
		{
			serverType = "Datacenter Edition for Itanium-based Systems";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
		{
			serverType = "Enterprise Edition for Itanium-based Systems";
		}
	}
	else if (sysInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
	{
		if (osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
		{
			serverType = "Datacenter x64 Edition";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
		{
			serverType = "Enterprise x64 Edition";
		}
		else 
		{
			serverType = "Standard x64 Edition";
		}
	}
	else
	{
		if (osVerInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
		{
			serverType = "Compute Cluster Edition";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_DATACENTER)
		{
			serverType = "Datacenter Edition";
		}
		else if(osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
		{
			serverType = "Enterprise Edition";
		}
		else if (osVerInfo.wSuiteMask & VER_SUITE_BLADE)
		{
			serverType = "Web Edition";
		}
		else 
		{
			serverType = "Standard Edition";
		}
	}

	return serverType;
}

QString getWindowsXp64OrServer2003(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	QString winVer("Windows XP 64 / Server 2003");
	
	if (GetSystemMetrics(SM_SERVERR2))
	{
		winVer = "Windows Server 2003 R2";
	}
	else if (osVerInfo.wSuiteMask == VER_SUITE_STORAGE_SERVER)
	{
		winVer = "Windows Storage Server 2003";
	}
	else if (osVerInfo.wSuiteMask == VER_SUITE_WH_SERVER)
	{
		winVer = "Windows Home Server";
	}
	else if(osVerInfo.wProductType == VER_NT_WORKSTATION &&
		sysInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
	{
		winVer = "Windows XP Professional x64 Edition";
	}
	else 
	{
		winVer = "Windows Server 2003";
	}

	if (osVerInfo.wProductType != VER_NT_WORKSTATION)
	{
		const QString serverType = getWindowsServer2003Type(osVerInfo, sysInfo);
		if (!serverType.isEmpty())
		{
			winVer = QString("%1 %2").arg(winVer).arg(serverType);
		}
	}

	return winVer;
}

QString getWindowsXpOrServer2003(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	if (osVerInfo.dwMinorVersion == 2)
	{
		return getWindowsXp64OrServer2003(osVerInfo, sysInfo);
	}
	else if (osVerInfo.dwMinorVersion == 1)
	{
		return getWindowsXp32(osVerInfo);
	}

	return QString("Windows XP / Server 2003");
}

QString getWindows2000(const OSVERSIONINFOEX& osVerInfo)
{
	if (osVerInfo.wProductType == VER_NT_WORKSTATION)
	{
		return QString("Windows 2000 Professional");
	}
	else
	{
		if (osVerInfo.wSuiteMask & VER_SUITE_DATACENTER )
		{
			return QString("Windows 2000 Datacenter Server");
		}
		else if (osVerInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
		{
			return QString( "Windows 2000 Advanced Server" );
		}
		else 
		{
			return QString( "Windows 2000 Server");
		}
	}
}

QString getWinXpOrWin2000OrWinServer2003(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	if (osVerInfo.dwMinorVersion == 0)
	{
		return getWindows2000(osVerInfo);
	}
	else if (osVerInfo.dwMinorVersion == 1)
	{
		return getWindowsXpOrServer2003(osVerInfo, sysInfo);
	}

	return QString("Windows XP / Windows 2000 / Windows Server 2003");
}

QString getProductType(const OSVERSIONINFOEX& osVerInfo)
{
	typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
	QString prodId;

	PGPI pGPI = (PGPI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
	if (!pGPI)
	{
		return prodId;
	}

	DWORD prodType = 0;
	if (pGPI(osVerInfo.dwMajorVersion, osVerInfo.dwMinorVersion, 0, 0, &prodType))
	{
		switch (prodType)
		{
		case PRODUCT_ULTIMATE:
			prodId = "Ultimate Edition";
			break;
		case PRODUCT_HOME_PREMIUM:
			prodId = "Home Premium Edition";
			break;
		case PRODUCT_HOME_BASIC:
			prodId = "Home Basic Edition";
			break;
		case PRODUCT_ENTERPRISE:
			prodId = "Enterprise Edition";
			break;
		case PRODUCT_BUSINESS:
			prodId = "Business Edition";
			break;
		case PRODUCT_STARTER:
			prodId = "Starter Edition";
			break;
		case PRODUCT_CLUSTER_SERVER:
			prodId = "Cluster Server Edition";
			break;
		case PRODUCT_DATACENTER_SERVER:
			prodId = "Datacenter Edition";
			break;
		case PRODUCT_DATACENTER_SERVER_CORE:
			prodId = "Datacenter Edition (core installation)";
			break;
		case PRODUCT_ENTERPRISE_SERVER:
			prodId = "Enterprise Edition";
			break;
		case PRODUCT_ENTERPRISE_SERVER_CORE:
			prodId = "Enterprise Edition (core installation)";
			break;
		case PRODUCT_ENTERPRISE_SERVER_IA64:
			prodId = "Enterprise Edition for Itanium-based Systems";
			break;
		case PRODUCT_SMALLBUSINESS_SERVER:
			prodId = "Small Business Server";
			break;
		case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			prodId = "Small Business Server Premium Edition";
			break;
		case PRODUCT_STANDARD_SERVER:
			prodId = "Standard Edition";
			break;
		case PRODUCT_STANDARD_SERVER_CORE:
			prodId = "Standard Edition (core installation)";
			break;
		case PRODUCT_WEB_SERVER:
			prodId = "Web Server Edition";
			break;
		}
	}

	return prodId;
}

QString getWindowsVistaOrWindows7OrServer2008(const OSVERSIONINFOEX& osVerInfo)
{
	QString winVer("Windows Vista / Windows 7 / Windows Server 2008 / Windows 8 / Windows Server 2012");

	if (osVerInfo.dwMinorVersion == 0)
	{
		if (osVerInfo.wProductType == VER_NT_WORKSTATION)
		{
			winVer = QString("Windows Vista");
		}
		else 
		{
			winVer = QString("Windows Server 2008");
		}
	}
	else if (osVerInfo.dwMinorVersion == 1)
	{
		if (osVerInfo.wProductType == VER_NT_WORKSTATION )
		{
			winVer = QString("Windows 7");
		}
		else
		{
			winVer = QString("Windows Server 2008 R2");
		}
	}
	else if (osVerInfo.dwMinorVersion == 2)
	{
		if (osVerInfo.wProductType == VER_NT_WORKSTATION )
		{
			winVer = QString("Windows 8");
		}
		else
		{
			winVer = QString("Windows Server 2012");
		}
	}
	
	const QString prodType = getProductType(osVerInfo);
	if (!prodType.isEmpty())
	{
		winVer = QString("%1 %2").arg(winVer).arg(prodType);
	}

	return winVer;
}

QString getWindowsNT(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	if (osVerInfo.dwMajorVersion > 6 || (osVerInfo.dwMajorVersion == 6 && osVerInfo.dwMinorVersion > 2) )
	{ // windows 8.1, 10 or greater
		QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", QSettings::NativeFormat);
		const QVariant productName = registry.value("ProductName");
		if (productName.isValid())
		{
			return productName.value<QString>();
		}

		if (osVerInfo.dwMajorVersion == 6 && osVerInfo.dwMinorVersion == 3)
		{
			return QString("Windows 8.1");
		}
		else if (osVerInfo.dwMajorVersion >= 10)
		{
			return QString("Windows 10 (or greater)");
		}
	}

	if (osVerInfo.dwMajorVersion == 3)
	{
		return QString("Windows NT 3.51");
	}
	else if (osVerInfo.dwMajorVersion == 4)
	{
		return QString("Windows NT 4.0");
	}
	else if (osVerInfo.dwMajorVersion == 5)
	{
		return getWinXpOrWin2000OrWinServer2003(osVerInfo, sysInfo);
	}
	else if (osVerInfo.dwMajorVersion == 6)
	{
		return getWindowsVistaOrWindows7OrServer2008(osVerInfo);
	}

	return QString("Windows NT Family");
}

QString getWindows9x(const OSVERSIONINFOEX& osVerInfo)
{
	QString winVer("Windows 9x");
	
	if (osVerInfo.dwMinorVersion < 10)
	{
		 winVer = QString("Windows 95");
	}
	else if (osVerInfo.dwMinorVersion == 10)
	{
		winVer = QString("Windows 98");
	}
	else if (osVerInfo.dwMinorVersion == 90)
	{
		winVer = QString("Windows Me");
	}

	return winVer;
}

QString getServicePackName(const OSVERSIONINFOEX& osVerInfo)
{
	const QString spName = QString::fromWCharArray((const wchar_t*)osVerInfo.szCSDVersion);
	return spName;
}

QString getBitVersion(const SYSTEM_INFO& sysInfo)
{
	QString arch;
	
	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
	{
		arch = "64-bit";
	}
	else if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
	{
		arch = "32-bit";
	}
	else
	{
		arch = QString("[Unknown bit version (%1)]").arg(sysInfo.wProcessorArchitecture);
	}

	return arch;
}

QString generateWindowsName(const OSVERSIONINFOEX& osVerInfo, const SYSTEM_INFO& sysInfo)
{
	QString winVer("Windows");

	if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		winVer = getWindows9x(osVerInfo);
	}
	else if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		winVer = getWindowsNT(osVerInfo, sysInfo);
	}

	const QString spName = getServicePackName(osVerInfo);
	if (!spName.isEmpty())
	{
		winVer = QString("%1 %2").arg(winVer).arg(spName);
	}

	winVer = QString("%1 (build %2)").arg(winVer).arg(osVerInfo.dwBuildNumber);

	const QString bitVersion = getBitVersion(sysInfo);
	if (!bitVersion.isEmpty())
	{
		winVer = QString("%1 %2").arg(winVer).arg(bitVersion);
	}

	return winVer;
}

QString getWindowsName()
{
	typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

 	OSVERSIONINFOEX osVerInfo;
 	SYSTEM_INFO sysInfo;
	PGNSI pGNSI;

 	ZeroMemory(&sysInfo, sizeof(SYSTEM_INFO));
 	ZeroMemory(&osVerInfo, sizeof(OSVERSIONINFOEX));

 	osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	const BOOL osVerInfoExSucceed = CompilerCompat::UndeprecatedGetVersionEx((OSVERSIONINFO*)&osVerInfo);
 	if (!osVerInfoExSucceed)
	{
		return QString("Windows");
	}	 
 
 	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise 
 	pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
 	if(pGNSI)
	{
		pGNSI(&sysInfo);
	}
 	else
	{
		GetSystemInfo(&sysInfo);
	}

	if (osVerInfo.dwMajorVersion == 6 && osVerInfo.dwMinorVersion == 2)
	{
		FileVersion kernelVersion = FileVersion::version(L"kernel32.dll");
		
		osVerInfo.dwMajorVersion = kernelVersion.majorVersion;
		osVerInfo.dwMinorVersion = kernelVersion.minorVersion;
		osVerInfo.dwBuildNumber = kernelVersion.buildNumber;
	}


	return generateWindowsName(osVerInfo, sysInfo);
}

}

#endif // Q_OS_WIN

QString SeoSpiderService::getOsName()
{
#ifdef Q_OS_WIN
	return getWindowsName();
#else
	return QString("Unknown");
#endif
}



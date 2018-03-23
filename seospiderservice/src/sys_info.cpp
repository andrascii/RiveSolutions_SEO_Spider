#include "sys_info.h"

#include "os_name.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACX)
#include <unistd.h>
#elif defined(Q_OS_MACX)
#include <mach/mach.h>
#include <mach/machine.h>
#elif defined(Q_OS_WIN)
#include <windows.h>
#include <psapi.h>
#include <intrin.h>

#include "get_version_windows.h"
#include "file_version.h"
#endif

namespace SeoSpiderService
{

void getUserInfo(SystemInformation::UserProfile& user)
{
	wchar_t buffer[MAX_PATH];
	DWORD length = MAX_PATH;
	if (::GetUserNameW(buffer, &length))
	{
		user.username = QString::fromWCharArray(buffer);
	}
	length = MAX_PATH;
	if (::GetComputerNameW(buffer, &length))
	{
		user.computer = QString::fromWCharArray(buffer);
	}
	user.language = QLocale::languageToString(QLocale::system().language());
	user.country = QLocale::countryToString(QLocale::system().country());
	user.utcOffset = QDateTime::currentDateTime().utcOffset();

	TIME_ZONE_INFORMATION timeZoneInfo;
	ZeroMemory(&timeZoneInfo, sizeof(timeZoneInfo));
	::GetTimeZoneInformation(&timeZoneInfo);

	user.languageId = ::GetUserDefaultLangID();
	user.gmtOffset = -timeZoneInfo.Bias;
}

void getOsInfo(SystemInformation::OperatingSystem& os)
{
	OSVERSIONINFO versionInfo;
	ZeroMemory(&versionInfo, sizeof(versionInfo));
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);

	CompilerCompat::UndeprecatedGetVersionEx(&versionInfo);

	// GetVersion/GetVersionEx:
	// Applications not manifested for Windows 8.1 or Windows 10 will return the Windows 8 OS version value (6.2)
	if (versionInfo.dwMajorVersion == 6)
	{
		FileVersion kernelVersion = FileVersion::version(L"kernel32.dll");

		os.majorVersion = kernelVersion.majorVersion;
		os.minorVersion = kernelVersion.minorVersion;
	}
	else
	{
		os.majorVersion = versionInfo.dwMajorVersion;
		os.minorVersion = versionInfo.dwMinorVersion;
	}

	os.patformId = versionInfo.dwPlatformId;
	os.version = QSysInfo::windowsVersion();
	os.name = getOsName();
}

QString getCpuVendorId()
{
	static const char* s_vendorId[][2]=
	{
		{"GenuineIntel", "Intel"},
		{"AuthenticAMD", "AMD"},
		{"CyrixInstead", "Cyrix"},
		{"CentaurHauls", "Centaur"},
		{"NexGenDriven", "NexGen"},
		{"GenuineTMx86", "Transmeta"},
		{"RiseRiseRise", "Rise"},
		{"UMC UMC UMC ", "UMC"},
		{"SiS SiS SiS ", "SiS"},
		{"Geode by NSC", "NSC"}
	};

	char buffer[20] = { 0 };
	const char* vendor = buffer;

#ifndef _MSC_VER
	__asm
	{
		pusha
		xor     eax,eax
		cpuid
		lea     esi,buffer
		mov     [esi],ebx
		mov     [esi+4],edx
		mov     [esi+8],ecx
		popa
	}
#else
	int CPUInfo[4] = {0};
	__cpuid(CPUInfo, 0);
	*((int*)buffer) = CPUInfo[1];
	*((int*)(buffer+4)) = CPUInfo[3];
	*((int*)(buffer+8)) = CPUInfo[2];
#endif

	for (int i = 0; i < _countof(s_vendorId); ++i)
	{
		if (0 == _stricmp(vendor, s_vendorId[i][0]))
		{
			vendor = s_vendorId[i][1];
			break;
		}
	}

	return QString::fromLatin1(vendor);
}

void getCpuId(quint64* id)
{
#ifndef _MSC_VER
	__asm
	{
		pusha
		mov		eax,1
		cpuid
		mov		esi,id
		mov		[esi],eax
		mov		[esi+4],ebx
		popa
	}
#else
	int CPUInfo[4] = {0};
	__cpuid(CPUInfo, 1);

	*id = CPUInfo[1];
	*id <<= 32;
	*id |= CPUInfo[0];
#endif
}

void getCpuInfo(SystemInformation::CPU& cpu)
{
	SYSTEM_INFO si;
	::GetSystemInfo(&si);
	cpu.cpuNum = si.dwNumberOfProcessors;
	getCpuId(&cpu.cpuId);
	cpu.vendorId = getCpuVendorId();
}

void getSystemMemoryInfo(SystemInformation::SystemMemory& sysmem)
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);

	if (::GlobalMemoryStatusEx(&ms))
	{
		sysmem.memoryLoad = ms.dwMemoryLoad;
		sysmem.totalPhysical = ms.ullTotalPhys;
		sysmem.availablePhysical = ms.ullAvailPhys;
		sysmem.totalPageFile = ms.ullAvailPageFile;
		sysmem.totalVirtual = ms.ullTotalVirtual;
		sysmem.availableVirtual = ms.ullAvailVirtual;
	}
}

bool queryProcessMemoryInfo(DWORD processId, PROCESS_MEMORY_COUNTERS& pmc)
{
	bool ret = false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processId);
	if (hProcess != NULL)
	{
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
		{
			ret = true;
		}
		CloseHandle(hProcess);
	}
	return ret;
}

void getProcessMemoryInfo(SystemInformation::ProcessMemory& pmem, quintptr pid)
{
	PROCESS_MEMORY_COUNTERS pmc;
	queryProcessMemoryInfo(pid, pmc);
	pmem.pageFaultCount = pmc.PageFaultCount;
	pmem.workingSetSize = pmc.WorkingSetSize;
	pmem.peakWorkingSetSize = pmc.PeakWorkingSetSize;
	pmem.quotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
	pmem.quotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
	pmem.quotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;
	pmem.quotaPeakNonPagedPoolUsage = pmc.QuotaPeakNonPagedPoolUsage;
	pmem.pagefileUsage = pmc.PagefileUsage;
	pmem.peakPagefileUsage = pmc.PeakPagefileUsage;
}

void getDisplayInfo(QVector<SystemInformation::Display>& displays)
{
	DISPLAY_DEVICEW device;
	ZeroMemory(&device, sizeof(device));
	device.cb = sizeof(device);
	QVector<DISPLAY_DEVICEW> deviceList;
	for (int i = 0; ; ++i)
	{
		if (!EnumDisplayDevicesW(NULL, i, &device, 0))
		{
			break;
		}
		if (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{
			deviceList.push_back(device);
		}
	}
	DEVMODEW mode;
	for (QVector<DISPLAY_DEVICEW>::const_iterator i = deviceList.begin();
		i != deviceList.end(); ++i)
	{
		if (!EnumDisplaySettingsW(i->DeviceName, ENUM_CURRENT_SETTINGS, &mode))
		{
			break;
		}
		SystemInformation::Display display;
		display.pixelHeight = mode.dmPelsHeight;
		display.pixelWidth = mode.dmPelsWidth;
		display.bitsPerPixel = mode.dmBitsPerPel;
		display.deviceName = QString::fromWCharArray(i->DeviceName);
		display.adapterName = QString::fromWCharArray(i->DeviceString);
		displays.push_back(display);
	}
}

VS_FIXEDFILEINFO getFileFixedInfo(const QFileInfo& fileInfo)
{
	DWORD dummy;
	DWORD fileInfoSize = ::GetFileVersionInfoSizeW(reinterpret_cast<LPCWSTR>(fileInfo.absoluteFilePath().utf16()), &dummy);
	VS_FIXEDFILEINFO ret = {0};
	if (0 == fileInfoSize)
	{
		return ret;
	}

	std::vector<unsigned char> data(fileInfoSize);

	if (!GetFileVersionInfoW(reinterpret_cast<LPCWSTR>(fileInfo.absoluteFilePath().utf16()), 0, fileInfoSize, &data[0])) 
	{
		return ret;
	}

	LPVOID ptr; UINT versize;
	if (!VerQueryValueW((const LPVOID)&data[0], L"\\", &ptr, &versize)) 
	{
		return ret;
	}

	ret = *(VS_FIXEDFILEINFO*) ptr;
	return ret;
}

QString getVersionString(const VS_FIXEDFILEINFO& fileinfo)
{
	unsigned majorVersion = HIWORD(fileinfo.dwFileVersionMS);
	unsigned minorVersion = LOWORD(fileinfo.dwFileVersionMS);
	unsigned majorBuild = HIWORD(fileinfo.dwFileVersionLS);
	unsigned minorBuild = LOWORD(fileinfo.dwFileVersionLS);
	return QString("%1.%2.%3.%4").arg(majorVersion).arg(minorVersion).arg(majorBuild).arg(minorBuild);
}

void getLoadedMoudules(QMap<QString, QString>& modules, quintptr pid)
{
	HANDLE process = ::OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid);
	if (process == NULL)
	{
		return;
	}
	DWORD size = 0;
	HMODULE list[1000];
	memset(list, 0, sizeof(list));
	::EnumProcessModules(process, list, sizeof(list), &size);
	int moduleCount = size / sizeof(HMODULE);
	for (int i = 0; i < moduleCount; ++i)
	{
		wchar_t moduleName[MAX_PATH]= {0};
		if (GetModuleFileNameExW(process, list[i], moduleName, MAX_PATH) == 0)
		{
			continue;
		}
		QFile file(QString::fromWCharArray(moduleName));
		QFileInfo fileInfo(file);
		if (!modules.contains(fileInfo.completeBaseName()))
		{
			VS_FIXEDFILEINFO verInfo = getFileFixedInfo(fileInfo);
			modules.insert(fileInfo.completeBaseName(), getVersionString(verInfo));
		}
	}
	CloseHandle(process);
}

void getEnvironmet(QMap<QString, QString>& environment)
{
	foreach(QString i, QProcess::systemEnvironment())
	{
		QStringList var = i.split("=");
		environment.insert(var[0], var[1]);
	}
}

bool getSystemInformation(SystemInformation* sysinfo, quintptr pid)
{
	Q_ASSERT(sysinfo);
	if (!sysinfo)
	{
		return false;
	}
	getUserInfo(sysinfo->userProfile);
	getOsInfo(sysinfo->operatingSystem);
	getCpuInfo(sysinfo->cpu);
	getSystemMemoryInfo(sysinfo->systemMemory);
	getProcessMemoryInfo(sysinfo->processMemory, pid);
	getDisplayInfo(sysinfo->displays);
	getLoadedMoudules(sysinfo->loadedModules, pid);
	getEnvironmet(sysinfo->environmentVariables);
	return true;
}

QString formatMemorySize(quint64 value)
{
	QString pattern;

	quint64 divisor = 1;

	if (value < 1024)
	{
		pattern = "%1 B";
		divisor = 1;
	}
	else if (value < (1024 * 1024))
	{
		pattern = "%1 KB";
		divisor = 1024;
	}
	else if (value < (1024 * 1024 * 1024))
	{
		pattern = "%1 MB";
		divisor = 1024*1024;
	}
	else
	{
		pattern = "%1 GB";
		divisor = 1024*1024*1024;
	}

	return pattern.arg(value / divisor);
}

bool printSystemInformation(QTextStream& out, quintptr pid)
{
	SystemInformation sysinfo;
	if (!getSystemInformation(&sysinfo, pid))
	{
		return false;
	}
	out << "[UserProfile]\n";
	out << "username=" << sysinfo.userProfile.username << '\n';
	out << "computername=" << sysinfo.userProfile.computer << '\n';
	out << "utcoffset=" << sysinfo.userProfile.utcOffset << '\n';
	out << "launguage=" << sysinfo.userProfile.language << '\n';
	out << "country=" << sysinfo.userProfile.country << '\n';

	out << "[OS]\n";
	out << "platformId=" << sysinfo.operatingSystem.patformId << '\n';
	out << "version=" << sysinfo.operatingSystem.version << '\n';
	out << "name=" << sysinfo.operatingSystem.name << '\n';

	out << "[CPU]\n";
	out << "cpuid=" << QString().sprintf("%08x.%08x", quint32(sysinfo.cpu.cpuId>>32), quint32(sysinfo.cpu.cpuId)) << '\n';
	out << "cpunum=" << sysinfo.cpu.cpuNum << '\n';
	out << "vendor=" << sysinfo.cpu.vendorId << '\n';

	out << "[System Memory]\n";
	out << "Physical memory load=" << sysinfo.systemMemory.memoryLoad << " %" << '\n';	
	out << "Available physical memory=" << formatMemorySize(sysinfo.systemMemory.availablePhysical) << '\n';
	out << "Available virtual memory=" << formatMemorySize(sysinfo.systemMemory.availableVirtual) << '\n';
	out << "Available page file=" << formatMemorySize(sysinfo.systemMemory.availablePageFile) << '\n';
	out << "Total physical memory=" << formatMemorySize(sysinfo.systemMemory.totalPhysical) << '\n';
	out << "Total virtual memory=" << formatMemorySize(sysinfo.systemMemory.totalVirtual) << '\n';
	out << "Total page file=" << formatMemorySize(sysinfo.systemMemory.totalPageFile) << '\n';

	out << "[Process Memory]\n";
	out << "PageFaultCount=" << formatMemorySize(sysinfo.processMemory.pageFaultCount) << '\n';
	out << "PagefileUsage=" << formatMemorySize(sysinfo.processMemory.pagefileUsage) << '\n';
	out << "PeakPagefileUsage=" << formatMemorySize(sysinfo.processMemory.peakPagefileUsage) << '\n';
	out << "PeakWorkingSetSize=" << formatMemorySize(sysinfo.processMemory.peakWorkingSetSize) << '\n';
	out << "QuotaNonPagedPoolUsage=" << formatMemorySize(sysinfo.processMemory.quotaNonPagedPoolUsage) << '\n';
	out << "QuotaPagedPoolUsage=" << formatMemorySize(sysinfo.processMemory.quotaPagedPoolUsage) << '\n';
	out << "QuotaPeakNonPagedPoolUsage=" << formatMemorySize(sysinfo.processMemory.quotaPeakNonPagedPoolUsage) << '\n';
	out << "QuotaPeakPagedPoolUsage=" << formatMemorySize(sysinfo.processMemory.quotaPeakPagedPoolUsage) << '\n';
	out << "WorkingSetSize=" << formatMemorySize(sysinfo.processMemory.workingSetSize) << '\n';
	
	out << "[Display]\n";
	for (int i = 0; i < sysinfo.displays.size(); ++i)
	{
		SystemInformation::Display& d = sysinfo.displays[i];
		out << "Num" << i << " Adapter=" << d.adapterName << '\n';
		out << "Num" << i << " Device=" << d.deviceName << '\n';
		out << "Num" << i << " Resolution=" << d.pixelWidth << "x" << d.pixelHeight << "x" << d.bitsPerPixel << '\n';
	}
	int dpiX = 0;
	int dpiY = 0;
	if (HDC hdc = GetDC(NULL))
	{
		dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
		dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);
	}
	out << "dpiX=" << dpiX << '\n';
	out << "dpiY=" << dpiY << '\n';
	
	out << "[Environment]\n";
	QMapIterator<QString, QString> e(sysinfo.environmentVariables);
	while (e.hasNext())
	{
		e.next();
		out << e.key() << "=" << e.value() << '\n';
	}
	
	out << "[Modules Info]\n";
	QMapIterator<QString, QString> m(sysinfo.loadedModules);
	while (m.hasNext())
	{
		m.next();
		out << m.key() << "=" << m.value() << "\n";
	}
	
	return true;
}

}

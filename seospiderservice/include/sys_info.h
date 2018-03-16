#pragma once
#ifndef SYSINFO_H
#define SYSINFO_H

namespace SeoSpiderService {

struct SystemInformation
{
	struct UserProfile
	{
		QString username;
		QString computer;
		QString language;
		quint32 languageId;
		QString country;
		quint32 utcOffset;
		quint32 gmtOffset;
	} userProfile;

	struct OperatingSystem
	{
		enum
		{
			Unknown = 0,
			Win = 1,
			Mac = 5
		};
		quint32 majorVersion;
		quint32 minorVersion;
		quint32 version;
		quint32 patformId;
		QString name;
	} operatingSystem;
	
	struct CPU
	{
		quint64 cpuId;
		quint32 cpuNum;
		QString vendorId;
	} cpu;
	
	struct SystemMemory
	{
		quint32 memoryLoad;
		quint64 totalPhysical;
		quint64 availablePhysical;
		quint64 totalPageFile;
		quint64 availablePageFile;
		quint64 totalVirtual;
		quint64 availableVirtual;
	} systemMemory;

	struct ProcessMemory
	{
		quint64 pageFaultCount;
		quint64 workingSetSize;
		quint64 peakWorkingSetSize;
		quint64 quotaPagedPoolUsage;
		quint64 quotaPeakPagedPoolUsage;
		quint64 quotaNonPagedPoolUsage;
		quint64 quotaPeakNonPagedPoolUsage;
		quint64 pagefileUsage;
		quint64 peakPagefileUsage;
	} processMemory;

	struct Display
	{
		quint32 pixelWidth;
		quint32 pixelHeight;
		quint32 bitsPerPixel;
		QString deviceName;
		QString adapterName;
	};

	QVector<Display> displays;
	QMap<QString, QString> loadedModules;
	QMap<QString, QString> environmentVariables;
};

bool getSystemInformation(SystemInformation* sysinfo, quintptr pid);
bool printSystemInformation(QTextStream& out, quintptr pid);

}

#endif//SYSINFO_H

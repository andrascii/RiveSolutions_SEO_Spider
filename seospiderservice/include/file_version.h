#pragma once
#include "stdafx.h"

#ifdef _WIN32

#include <windows.h>

struct FileVersion
{
	DWORD majorVersion;
	DWORD minorVersion;
	DWORD buildNumber;
	DWORD revisionNumber;

	WORD language;
	WORD codePage;

	QString productString;

	FileVersion();

	static FileVersion version(const wchar_t* moduleName);
};

#endif


struct FileVersionInfo
{
	QString versionString;
	QString productString;
};

bool getFileVersionInfo(const QString& fileName, FileVersionInfo* info);

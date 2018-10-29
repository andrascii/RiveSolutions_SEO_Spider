#include "file_version.h"

#ifndef _WIN32
#error "Only for windows"
#endif

struct LangAndCodePage
{
	WORD language;
	WORD codePage;
};

FileVersion::FileVersion()
	: majorVersion(0)
	, minorVersion(0)
	, buildNumber(0)
	, revisionNumber(0)
	, language(0)
	, codePage(0)
{}

FileVersion FileVersion::version(const wchar_t* moduleName)
{
	FileVersion fileVersion;

	DWORD versionHandle;

	const DWORD infoSize = GetFileVersionInfoSizeW(moduleName, &versionHandle);
	if (infoSize == 0)
	{
		qWarning("GetFileVersionInfoSize failed %u", GetLastError());
		return fileVersion;
	}

	QByteArray buffer(infoSize, (char)0);

	if (GetFileVersionInfoW(moduleName, versionHandle, infoSize, buffer.data()) != BOOL(TRUE))
	{
		qWarning("GetFileVersionInfo failed %u", GetLastError());
		return fileVersion;
	}

	UINT size = 0;

	VS_FIXEDFILEINFO* fileInfo = NULL;

	bool success =
		VerQueryValueW(buffer.data(), L"\\", reinterpret_cast<void**>(&fileInfo), &size) == BOOL(TRUE);

	if (success)
	{
		fileVersion.majorVersion = HIWORD(fileInfo->dwFileVersionMS);
		fileVersion.minorVersion = LOWORD(fileInfo->dwFileVersionMS);
		fileVersion.buildNumber = HIWORD(fileInfo->dwFileVersionLS);
		fileVersion.revisionNumber = LOWORD(fileInfo->dwFileVersionLS);
	}
	else
	{
		qWarning("VerQueryValue '\\' failed %u", GetLastError());

		return fileVersion;
	}


	//The Value member to contain an array of DWORD values indicating the language and code page combinations supported by this file.
	//The low-order word of each DWORD must contain a Microsoft language identifier,
	//and the high-order word must contain the IBM code page number.
	LangAndCodePage* languageInfo = NULL;

	success =
		VerQueryValueW(buffer.data(), L"\\VarFileInfo\\Translation", reinterpret_cast<void**>(&languageInfo), &size) == BOOL(TRUE);

	if (success)
	{
		fileVersion.language = languageInfo->language;
		fileVersion.codePage = languageInfo->codePage;

		const QString blockName =
			QString()
				.sprintf("\\StringFileInfo\\%04x%04x\\ProductName", fileVersion.language, fileVersion.codePage);

		wchar_t* productString = NULL;

		if (VerQueryValueW(
					buffer.data(),
					blockName.toStdWString().c_str(),
					reinterpret_cast<void**>(&productString),
					&size
		) == BOOL(TRUE) )
		{
			fileVersion.productString = QString::fromWCharArray(productString);
		}
		else
		{
			qWarning("VerQueryValue %ls failed %u", blockName.utf16(), GetLastError());
		}
	}
	else
	{
		qWarning("VerQueryValue \\VarFileInfo\\Translation failed %u", GetLastError());
	}

	return fileVersion;
}

//

bool getFileVersionInfo(const QString& fileName, FileVersionInfo* info)
{
	if (info == NULL)
	{
		return false;
	}

	FileVersion ver = FileVersion::version(fileName.toStdWString().c_str());

	info->versionString = QString("%1.%2.%3.%4")
		.arg(ver.majorVersion)
		.arg(ver.minorVersion)
		.arg(ver.buildNumber)
		.arg(ver.revisionNumber);

	info->productString = ver.productString;

	return true;
}

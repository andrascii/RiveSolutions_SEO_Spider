#pragma once
#include "stdafx.h"

#ifdef _MSC_VER

#include <windows.h>

namespace CompilerCompat
{

// https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451%28v=vs.85%29.aspx
// GetVersionEx is deprecated
// With the release of Windows 8.1, the behavior of the GetVersionEx API has changed in the value it will
//  return for the operating system version. The value returned by the GetVersionEx function now depends
//  on how the application is manifested.
// Anyway we'll get VersionInfo from any source without warning
// http://stackoverflow.com/questions/20031597/error-c4996-received-when-compiling-sqlite-c-in-visual-studio-2013

inline BOOL UndeprecatedGetVersionEx(OSVERSIONINFO* osVerInfo)
{
#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'GetVersionExW': was declared deprecated
	return GetVersionEx(osVerInfo);
#pragma warning(pop)
}

inline BOOL UndeprecatedGetVersionExA(OSVERSIONINFOA* osVerInfo)
{
#pragma warning(push)
#pragma warning(disable:4996) // warning C4996: 'GetVersionExA': was declared deprecated
	return GetVersionExA(osVerInfo);
#pragma warning(pop)
}

}

#endif

#pragma once

namespace Common
{

class DebugHelpersApi
{
public:
	static void doAssert(const char* file, int line, const char* function, const char* expression);
	static void debugReport(const char* file, int line, const char* function, const char* expression);
	static void debugBreak();
};

}

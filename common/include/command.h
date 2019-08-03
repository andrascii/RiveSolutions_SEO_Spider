#pragma once
#include "stdafx.h"

namespace Common
{

typedef int CategoryId;

#undef max
#define max(a, b) (((a) < (b)) ? (b) : (a))

//! Log level
enum LogLevel
{
	//! Detailed information
	VerboseLog = 0x1,
	//! Standard level (default)
	DebugLog = 0x2,
	//! Warnings
	WarningLog = 0x3,
	//! Errors
	ErrorLog = 0x4,
	//! Information
	InfoLog = 0x5,
	//! Mask for level
	LevelMask = 0x7,
	//! Ignore all messages
	DisabledLog = 0x80,
	//! Use parent log level
	InheritedLogLevel = 0x100,
	//! Counter log
	CounterLog = 0x110,
	//! Application Initialized
	ApplicationInitialized = 0x120
};

inline QLatin1String logLevelName(LogLevel f)
{
	switch (f)
	{
		case VerboseLog:
			return QLatin1String("Verbose");

		case DebugLog:
			return QLatin1String("Debug");

		case WarningLog:
			return QLatin1String("Warning");

		case ErrorLog:
			return QLatin1String("Error");

		case InfoLog:
			return QLatin1String("Info");

		case DisabledLog:
			return QLatin1String("Disabled");

		case InheritedLogLevel:
			return QLatin1String("Inherited");
	}
	return QLatin1String("invalid_loglevel_value");
}

//! Log data structure
struct LogData
{
	static const size_t maximumTextLength = 2 * 1024;
	//! Copies text body
	static void setText(LogData* logData, const char* text);
	//! Log category identifier
	CategoryId categoryId;
	//! Command severity level
	int level;
	//! Source file
	char file[256];
	//! Source line
	unsigned int line;
	//! Function or method name
	char function[256];
	//! Class this pointer or null if called from function
	/*! Note: Supported only by MSVC compiler */
	const void* thisptr;
	//! target thread ID
	const void* threadId;
	//! Function call depth
	int depth;
	//! Log message text
	char text[maximumTextLength + 1];
};

//! Assert data structure.
struct AssertData
{
	//! Source file.
	char file[1024];
	//! Source line.
	unsigned int line;
	//! Source function.
	char function[1024];
	//! Class this pointer or null if called from function.
	/*! Note: Supported only by MSVC compiler. */
	const void* thisptr;
	//! Assert condition text.
	char condition[1024];
	//! Target process identifier.
	quintptr processId;
	//! Thread identifier with generate exception.
	const void* threadId;
	//! Raw exception data.
	const void* exceptionInfo;
	qint32 exceptionSize;
};

//! Dump data structure
struct DumpData
{
	enum Params { UserDump = 0, NativeCrash = 1 };
	//! Target process identifier.
	quintptr processId;
	//! Thread identifier with generate exception.
	const void* threadId;
	//! Size of exceptionInfo data.
	/*! Command handler detect the type of exceptinInfo data by comparing size with
	predefined values on each platform. */
	qint32 exceptionSize;
	qint32 dumpParams;
	//! Raw exception data.
	const void* exceptionInfo;
	char filelist[2000];
};

struct CategoryData
{
	char categoryName[256];
};

struct RestartData
{
	char message[256];
	//! buffer for command line arguments (we must have sizeof(RestartData) < DatagramSizeLimit)
	char cmdlineBuffer[3 * 1024];
	//! Tokens offset in cmdlineBuffer. We limit number of arguments for restart by 256 params
	//! int32 used for decrease RestartData size in 64 bit
	//! -1 value used as terminated item
	qintptr cmdlineTokens[256];
	bool showDialog;
	int timeout;
};

struct CounterData
{
	enum Type
	{
		SessionId = 0,
		UseCounter = 1,
		TimerCounter = 2
	};
	char name[256];
	quint64 value;
	Type type;
};

struct ApplicationInitializedData
{
	char userID[256];
	char country[128];
	char language[64];
	char os[128];
	char programBittness[64];
	char programVersion[128];
};

//! IPC communication message structure
/*! Due to system requirement message size limited to 8192 bytes. */
struct Command
{
	//! Command type
	enum Type
	{
		//! Undefined value
		Undef = 0,
		//! Assert
		Assert,
		//! Dump
		Dump,
		//! Log write
		Log,
		//! Define new category
		LogCategory,
		//! Request to restart watching application
		Restart,
		//! Counter update
		Counter,
		//! Application Initialized
		ApplicationInitialized
	};

	inline LogData* logData() { return type == Log ? reinterpret_cast<LogData*>(rawData) : 0; }
	inline AssertData* assertData() { return type == Assert ? reinterpret_cast<AssertData*>(rawData) : 0; };
	inline DumpData* dumpData() { return type == Dump ? reinterpret_cast<DumpData*>(rawData) : 0; };
	inline CategoryData* categoryData() { return type == LogCategory ? reinterpret_cast<CategoryData*>(rawData) : 0; }
	inline RestartData* restartData() { return type == Restart ? reinterpret_cast<RestartData*>(rawData) : 0; }
	inline CounterData* counterData() { return type == Counter ? reinterpret_cast<CounterData*>(rawData) : 0; }
	inline ApplicationInitializedData* applicationInitializedData() { return type == ApplicationInitialized ? reinterpret_cast<ApplicationInitializedData*>(rawData) : 0; }

	void setLogData(const char* file, int line, const char* function, const void* thisptr);
	void setDumpData(const void* exceptionInfo, int exceptionSize, int params);
	void setAssertData(const char* file, int line, const char* function,
		const void* thisptr, const char* condition, void* exceptionInfo, int exceptionSize);
	void setCategoryData(const char* categoryName);
	void setRestartData(const char* displayMessage, bool showRestartDialog, int timeout, size_t argc, const char** argv);
	void setCounterData(const char* name, quint64 value, int counterType);
	void setApplicationsInitializedData(const char* userID, const char* country, const char* language, const char* OS, const char* programBittness, const char* programVersion);

	//! Command type
	Type type;
	static const size_t rawDataSize =
		max(sizeof(LogData),
		max(sizeof(AssertData),
		max(sizeof(DumpData),
		max(sizeof(CategoryData),
		max(sizeof(RestartData),
		max(sizeof(ApplicationInitializedData), sizeof(CounterData)))))));

	//! Raw data structure on of LogData, AssertData or DumpData
	char rawData[rawDataSize];
};

#undef max

//! Command execution result Response structure
struct Result
{
	//! Error code
	int errorcode;
	union
	{
		CategoryId categoryId;
	} data;
};

}

Q_DECLARE_METATYPE(Common::Command);
Q_DECLARE_METATYPE(Common::LogLevel);
#include "seo_spider_service_api_impl.h"
#include "iipc_signaled_object.h"
#include "ipc_signaled_object_creator.h"
#include "seo_spider_service_api_export.h"
#include "command.h"

using namespace Common;

namespace
{

const std::uint8_t s_interceptFunctionOpcodes[] =
{
#ifdef _WIN64
	//  XOR RAX, RAX
	//  RET
	0x48, 0x31, 0xC0, 0xC3
#else
	//  XOR EAX, EAX
	//  RET 4
	0x33, 0xC0, 0xC2, 0x04, 0x00
#endif
};

class MessageBoxThread final
{
public:
	MessageBoxThread(const std::string& text)
		: m_text(text)
		, m_result(IDRETRY)
	{
	}

	int askUser()
	{
		std::thread messageBoxThread = std::thread(&MessageBoxThread::run, this);
		messageBoxThread.join();

		return m_result;
	}

protected:

	void run()
	{
		
#if defined(QT_DEBUG)
		long messageBoxType = MB_RETRYCANCEL;
#else
		long messageBoxType = MB_OK;
#endif

		messageBoxType = messageBoxType | MB_ICONSTOP | MB_TOPMOST;

		m_result = MessageBoxA(0, m_text.c_str(), "SeoSpiderServiceApi Debug Library!", messageBoxType);
	}

private:
	int m_result;
	const std::string m_text;
};

void qtMsgHandler(QtMsgType type, const QMessageLogContext&, const QString& msg)
{
#ifdef QT_DEBUG
	if (msg.contains("signal not found"))
	{
		type = QtCriticalMsg;
	}
#endif

	switch (type)
	{
	case QtDebugMsg:
		seoSpiderServiceApi()->writeLog(
			1,
			Common::LogLevel::DebugLog,
			__FILENAME__,
			__LINE__,
			__FUNCTION__,
			nullptr,
			msg.toStdString().c_str()
		);
		break;

	case QtWarningMsg:
		seoSpiderServiceApi()->writeLog(
			1,
			Common::LogLevel::WarningLog,
			__FILENAME__,
			__LINE__,
			__FUNCTION__,
			nullptr,
			msg.toStdString().c_str()
		);
		break;

	case QtCriticalMsg:
		seoSpiderServiceApi()->writeLog(
			1,
			Common::LogLevel::ErrorLog,
			__FILENAME__,
			__LINE__,
			__FUNCTION__,
			nullptr,
			msg.toStdString().c_str()
		);
		break;

	case QtFatalMsg:
		seoSpiderServiceApi()->writeLog(
			1,
			Common::LogLevel::ErrorLog,
			__FILENAME__,
			__LINE__,
			__FUNCTION__,
			nullptr,
			msg.toStdString().c_str()
		);
		abort();
		break;
	}
}

std::function<bool(Common::LogLevel)> createLogFilter()
{
	return [](Common::LogLevel level)
	{
#ifdef QT_DEBUG
		level;

		return true;
#else
		if (level < Common::LogLevel::WarningLog)
		{
			return false;
		}

		return true;
#endif
	};
}

size_t logDataPacketSize(const char* text)
{
	const size_t textLength = std::strlen(text);

	if (textLength <= LogData::maximumTextLength)
	{
		return sizeof(Common::Command);
	}

	const size_t result = sizeof(Common::Command) + sizeof(wchar_t) * (textLength - Common::LogData::maximumTextLength + 1);

	return result;
}

PCHAR*
CommandLineToArgvA(
	PCHAR CmdLine,
	int* _argc
)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = (ULONG)strlen(CmdLine);
	i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2) * sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i])
	{
		if (in_QM)
		{
			if (a == '\"')
			{
				in_QM = FALSE;
			}
			else
			{
				_argv[j] = a;
				j++;
			}
		}
		else
		{
			switch (a)
			{
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE)
					{
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT)
					{
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE)
					{
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

}

namespace SeoSpiderServiceApi
{

SeoSpiderServiceApiImpl* SeoSpiderServiceApiImpl::s_self = nullptr;

SeoSpiderServiceApiImpl::SeoSpiderServiceApiImpl()
	: m_startupInfo{ sizeof(m_startupInfo) }
	, m_initialized(false)
{
	ASSERT(s_self == nullptr && "Double instances detected!");

	m_logFilter.setFilter(createLogFilter());

	s_self = this;
}

void SeoSpiderServiceApiImpl::init()
{
	if (m_initialized)
	{
		return;
	}

	std::wstring commandLine = L"seospiderservice.exe " + std::to_wstring(GetCurrentProcessId());

	m_initialized = CreateProcessW(
		NULL,
		commandLine.data(),
		NULL,
		NULL,
		TRUE,
		CREATE_NO_WINDOW,
		NULL,
		NULL,
		&m_startupInfo,
		&m_processInfo
	);

	if (m_initialized)
	{
		m_pipeServer.reset(new PipeServer);
	}

	qInstallMessageHandler(qtMsgHandler);
}

void SeoSpiderServiceApiImpl::free() const
{
	if (!m_initialized)
	{
		return;
	}

	CloseHandle(m_processInfo.hThread);
	CloseHandle(m_processInfo.hProcess);
}

bool SeoSpiderServiceApiImpl::restartApplication(int msec)
{
	Command command;
	int argc = 0;

	const char** argv = (const char**)CommandLineToArgvA(::GetCommandLineA(), &argc);
	command.setRestartData("", false, msec, argc, argv);
	::LocalFree(argv);

	Result result;
	result.errorcode = -1;
	m_pipeServer->transactData((const char*)&command, sizeof(Command), (char*)&result, sizeof(Result));
	return result.errorcode == 0;
}

bool SeoSpiderServiceApiImpl::doDump(const void* exceptionInfo, const int exceptionInfoSize)
{
	return makeCrashDump(exceptionInfo, exceptionInfoSize);
}

void SeoSpiderServiceApiImpl::setProcessExceptionHandlers() const
{
	SetUnhandledExceptionFilter(SeoSpiderServiceApiImpl::exceptionFilter);
	lockExceptionFilter();

	_set_purecall_handler(SeoSpiderServiceApiImpl::pureCallHandler);
	_set_new_handler(SeoSpiderServiceApiImpl::newHandler);
	_set_invalid_parameter_handler(SeoSpiderServiceApiImpl::invalidParameterHandler);
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	std::signal(SIGABRT, SeoSpiderServiceApiImpl::sigAbortHandler);
	std::signal(SIGINT, SeoSpiderServiceApiImpl::sigIntHandler);
	std::signal(SIGTERM, SeoSpiderServiceApiImpl::sigTermHandler);
}

void SeoSpiderServiceApiImpl::setThreadExceptionHandlers() const
{
	typedef void(*SignalHandlerFunctionType)(int);

	set_terminate(SeoSpiderServiceApiImpl::terminateHandler);
	set_unexpected(SeoSpiderServiceApiImpl::unexpectedHandler);

	std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(SeoSpiderServiceApiImpl::sigFpeHandler));
	std::signal(SIGILL, SeoSpiderServiceApiImpl::sigIllHandler);
	std::signal(SIGSEGV, SeoSpiderServiceApiImpl::sigSegvHandler);
}

void SeoSpiderServiceApiImpl::doAssert(const char* file, int line, const char* function, const char* expression)
{
#ifndef PRODUCTION
	debugReport(file, line, function, expression);
#else
	__try
	{
		// generate minidump exception
		DebugBreak();
	}
	__except (assertExceptionFilter(file, line, function, this, expression, GetExceptionInformation()))
	{
	}

	TerminateProcess(GetCurrentProcess(), 0xDEAD);
#endif
}

void SeoSpiderServiceApiImpl::setLogFilter(const std::function<bool(Common::LogLevel)>& filter)
{
	m_logFilter.setFilter(filter);
}

bool SeoSpiderServiceApiImpl::writeLog(
	int id,
	int level,
	const char* file,
	unsigned int line,
	const char* function,
	const void* thisptr,
	const char* text)
{
	if (!m_logFilter.isAvailable(static_cast<Common::LogLevel>(level)))
	{
		return false;
	}

	const size_t packetSize = logDataPacketSize(text);

	if (packetSize > sizeof(Command))
	{
		return writeLogHeap(id, level, file, line, function, thisptr, text);
	}

	return writeLogStack(id, level, file, line, function, thisptr, text);
}

void SeoSpiderServiceApiImpl::debugReport(const char* file, int line, const char* function, const char* expression) const
{
	std::stringstream text;

	text << std::string("Debug Assertion") << std::endl << std::endl;
	text << std::string("Thread: ") << (ptrdiff_t)QThread::currentThreadId() << std::endl;
	text << std::string("Function: ") << function << std::endl;
	text << std::string("File: ") << file << std::endl;
	text << std::string("Line: ") << line << std::endl << std::endl;
	text << std::string("Expression: ") << expression << std::endl << std::endl;
	text << std::string("Press Retry to debug the application");

	// Run message box in background thread to stop main thread message pump
	MessageBoxThread msgbox(text.str());

	switch (msgbox.askUser())
	{
		case IDRETRY:
		{
			DebugBreak();
			break;
		}
		case IDCANCEL:
		{
			TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
			break;
		}
	}
}

LONG WINAPI SeoSpiderServiceApiImpl::exceptionFilter(PEXCEPTION_POINTERS pExceptionPtrs)
{
	s_self->doDump(pExceptionPtrs, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);

	return EXCEPTION_EXECUTE_HANDLER;
}

void SeoSpiderServiceApiImpl::terminateHandler()
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::unexpectedHandler()
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::pureCallHandler()
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigAbortHandler(int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigFpeHandler(int, int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigIntHandler(int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigIllHandler(int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigSegvHandler(int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

void SeoSpiderServiceApiImpl::sigTermHandler(int)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

int SeoSpiderServiceApiImpl::newHandler(size_t)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
	return 0;
}

void SeoSpiderServiceApiImpl::invalidParameterHandler(
	const wchar_t*,
	const wchar_t*,
	const wchar_t*,
	unsigned int,
	uintptr_t)
{
	s_self->doDump(nullptr, 0);
	TerminateProcess(GetCurrentProcess(), 0xDEAD);
}

bool SeoSpiderServiceApiImpl::lockExceptionFilter()
{
	//
	// We need to patch SetUnhandledExceptionFilter because
	// CRT library calls this function with 0 as a parameter
	// And it clears the already installed SEH handler
	//

	HMODULE kernel32 = GetModuleHandleW(L"KERNEL32.DLL");

	if (kernel32 == nullptr)
	{
		return false;
	}

	void* entryPoint = static_cast<void*>(GetProcAddress(kernel32, "SetUnhandledExceptionFilter"));

	if (entryPoint == nullptr)
	{
		return false;
	}

	DWORD oldProtect = 0;

	if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE)
	{
		return false;
	}

	std::memcpy(entryPoint, s_interceptFunctionOpcodes, sizeof(s_interceptFunctionOpcodes));
	DWORD temp = 0;

	if (VirtualProtect(entryPoint, sizeof(s_interceptFunctionOpcodes), oldProtect, &temp) == FALSE)
	{
		return false;
	}

	return true;
}

unsigned SeoSpiderServiceApiImpl::assertExceptionFilter(
	const char* file,
	int line,
	const char* function,
	const void* thisptr,
	const char* expression,
	void* exceptionInfo)
{
	EXCEPTION_POINTERS* ep = (EXCEPTION_POINTERS*)exceptionInfo;
	Command command;
	command.setAssertData(file, line, function, thisptr, expression, ep, sizeof(*ep));
	Result result;
	m_pipeServer->transactData((const char*)&command, sizeof(Command), (char*)&result, sizeof(Result));
	return EXCEPTION_EXECUTE_HANDLER;
}

bool SeoSpiderServiceApiImpl::writeLogStack(
	int id,
	int level,
	const char* file,
	unsigned int line,
	const char* function,
	const void* thisptr,
	const char* text)
{
	Common::Command command;
	command.setLogData(file, line, function, thisptr);
	Common::LogData* logData = command.logData();
	logData->categoryId = id;
	logData->level = level;
	logData->depth = 0;
	Common::LogData::setText(logData, text);

	return m_pipeServer->writeData((const char*)&command, sizeof(Common::Command)) == sizeof(Common::Command);
}

bool SeoSpiderServiceApiImpl::writeLogHeap(
	int id,
	int level,
	const char* file,
	unsigned int line,
	const char* function,
	const void* thisptr,
	const char* text)
{
	const size_t bufferSize = logDataPacketSize(text);
	std::vector<char> buffer(bufferSize);

	Common::Command* command = reinterpret_cast<Common::Command*>(&buffer[0]);
	command->setLogData(file, line, function, thisptr);
	Common::LogData* logData = command->logData();
	logData->categoryId = id;
	logData->level = level;
	logData->depth = 0;
	Common::LogData::setText(logData, text);

	return m_pipeServer->writeData(&buffer[0], buffer.size()) == static_cast<qint64>(buffer.size());
}

bool SeoSpiderServiceApiImpl::makeCrashDump(const void* exceptionInfo, const int exceptionInfoSize)
{
	Command command;
	command.setDumpData(exceptionInfo, exceptionInfoSize, DumpData::NativeCrash);

	Result result;
	
	m_pipeServer->transactData((const char*)&command, sizeof(Command), (char*)&result, sizeof(Result));
	return result.errorcode == 0;
}

}
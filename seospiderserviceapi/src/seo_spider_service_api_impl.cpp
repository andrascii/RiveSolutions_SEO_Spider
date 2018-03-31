#include "seo_spider_service_api_impl.h"
#include "iipc_signaled_object.h"
#include "ipc_signaled_object_creator.h"
#include "seo_spider_service_api_export.h"

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
		seoSpiderServiceApi()->debugLogMessage(
			Common::PipeMessage::LogMessage, 
			std::hash<std::thread::id>{}(std::this_thread::get_id()),
			__LINE__,
			__FILENAME__,
			__FUNCTION__,
			msg.toStdString().c_str()
		);
		break;

	case QtWarningMsg:
		seoSpiderServiceApi()->warningLogMessage(
			Common::PipeMessage::LogMessage,
			std::hash<std::thread::id>{}(std::this_thread::get_id()),
			__LINE__,
			__FILENAME__,
			__FUNCTION__,
			msg.toStdString().c_str()
		);
		break;

	case QtCriticalMsg:
		seoSpiderServiceApi()->errorLogMessage(
			Common::PipeMessage::LogMessage,
			std::hash<std::thread::id>{}(std::this_thread::get_id()),
			__LINE__,
			__FILENAME__,
			__FUNCTION__,
			msg.toStdString().c_str()
		);
		break;

	case QtFatalMsg:
		seoSpiderServiceApi()->errorLogMessage(
			Common::PipeMessage::LogMessage,
			std::hash<std::thread::id>{}(std::this_thread::get_id()),
			__LINE__,
			__FILENAME__,
			__FUNCTION__,
			msg.toStdString().c_str()
		);
		abort();
		break;
	}
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

	s_self = this;
}

void SeoSpiderServiceApiImpl::init() noexcept
{
	if (m_initialized)
	{
		return;
	}

	const boost::uuids::uuid uuid = boost::uuids::random_generator()();
	const std::wstring crashHandlerEventName = boost::lexical_cast<std::wstring>(uuid);
	std::wstring commandLine = L"seospiderservice.exe " + crashHandlerEventName + L" " + std::to_wstring(GetCurrentProcessId());

	m_crashEventSignaledObject = Common::createSignaledObjectInstance();
	const bool creationResult = m_crashEventSignaledObject->create(crashHandlerEventName);

	ASSERT(creationResult);

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

	m_pipeServer.reset(new PipeServer);

	qInstallMessageHandler(qtMsgHandler);

	if (!m_initialized)
	{
		CloseHandle(m_processInfo.hThread);
		CloseHandle(m_processInfo.hProcess);
	}
}

void SeoSpiderServiceApiImpl::free() const noexcept
{
	if (!m_initialized)
	{
		return;
	}
}

void SeoSpiderServiceApiImpl::setProcessSignaledState() const noexcept
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_crashEventSignaledObject->setSignaledState();
	m_pipeServer->closeConnection();
	Sleep(INFINITE);
}

void SeoSpiderServiceApiImpl::setProcessExceptionHandlers() const noexcept
{
	SetUnhandledExceptionFilter(SeoSpiderServiceApiImpl::sehHandler);
	lockExceptionFilter();

	_set_purecall_handler(SeoSpiderServiceApiImpl::pureCallHandler);
	_set_new_handler(SeoSpiderServiceApiImpl::newHandler);
	_set_invalid_parameter_handler(SeoSpiderServiceApiImpl::invalidParameterHandler);
	_set_abort_behavior(_CALL_REPORTFAULT, _CALL_REPORTFAULT);

	std::signal(SIGABRT, SeoSpiderServiceApiImpl::sigAbortHandler);
	std::signal(SIGINT, SeoSpiderServiceApiImpl::sigIntHandler);
	std::signal(SIGTERM, SeoSpiderServiceApiImpl::sigTermHandler);
}

void SeoSpiderServiceApiImpl::setThreadExceptionHandlers() const noexcept
{
	typedef void(*SignalHandlerFunctionType)(int);

	set_terminate(SeoSpiderServiceApiImpl::terminateHandler);
	set_unexpected(SeoSpiderServiceApiImpl::unexpectedHandler);

	std::signal(SIGFPE, reinterpret_cast<SignalHandlerFunctionType>(SeoSpiderServiceApiImpl::sigFpeHandler));
	std::signal(SIGILL, SeoSpiderServiceApiImpl::sigIllHandler);
	std::signal(SIGSEGV, SeoSpiderServiceApiImpl::sigSegvHandler);
}

void SeoSpiderServiceApiImpl::doAssert(const char* file, int line, const char* function, const char* expression) const noexcept
{
#ifndef PRODUCTION
	debugReport(file, line, function, expression);
#else
	Q_UNUSED(file);
	Q_UNUSED(line);
	Q_UNUSED(function);
	Q_UNUSED(expression);

	setProcessSignaledState();
#endif
}

void SeoSpiderServiceApiImpl::debugReport(const char* file, int line, const char* function, const char* expression) const noexcept
{
	m_pipeServer->logMessage(
		Common::PipeMessage::Assert,
		Common::SeverityLevel::ErrorLevel,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		line,
		file,
		function,
		expression
	);

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

void SeoSpiderServiceApiImpl::traceLogMessage(
	Common::PipeMessage::Type type,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	m_pipeServer->logMessage(type, Common::SeverityLevel::TraceLevel, threadId, line, file, function, message);
}

void SeoSpiderServiceApiImpl::debugLogMessage(
	Common::PipeMessage::Type type,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	m_pipeServer->logMessage(type, Common::SeverityLevel::DebugLevel, threadId, line, file, function, message);
}

void SeoSpiderServiceApiImpl::infoLogMessage(
	Common::PipeMessage::Type type,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	m_pipeServer->logMessage(type, Common::SeverityLevel::InfoLevel, threadId, line, file, function, message);
}

void SeoSpiderServiceApiImpl::warningLogMessage(
	Common::PipeMessage::Type type,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	m_pipeServer->logMessage(type, Common::SeverityLevel::WarningLevel, threadId, line, file, function, message);
}

void SeoSpiderServiceApiImpl::errorLogMessage(
	Common::PipeMessage::Type type,
	std::uint64_t threadId,
	std::uint64_t line,
	const char* file,
	const char* function,
	const char* message)
{
	m_pipeServer->logMessage(type, Common::SeverityLevel::ErrorLevel, threadId, line, file, function, message);
}

LONG WINAPI SeoSpiderServiceApiImpl::sehHandler(PEXCEPTION_POINTERS)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()), 
		__LINE__, 
		__FILENAME__, 
		__FUNCTION__,
		"structured exception handler"
	);

	s_self->setProcessSignaledState();
	return EXCEPTION_EXECUTE_HANDLER;
}

void SeoSpiderServiceApiImpl::terminateHandler()
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"terminate handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::unexpectedHandler()
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"unexcepted handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::pureCallHandler()
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"pure call handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigAbortHandler(int)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"abort signal handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigFpeHandler(int code, int subcode)
{
	Q_UNUSED(code);
	Q_UNUSED(subcode);

	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"floating point signal handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigIntHandler(int)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"interruption signal handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigIllHandler(int)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"illegal instruction signal handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigSegvHandler(int)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"segmentation fault signal handler"
	);

	s_self->setProcessSignaledState();
}

void SeoSpiderServiceApiImpl::sigTermHandler(int)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"terminate signal handler"
	);

	s_self->setProcessSignaledState();
}

int SeoSpiderServiceApiImpl::newHandler(size_t)
{
	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"new handler"
	);

	s_self->setProcessSignaledState();
	return 0;
}

void SeoSpiderServiceApiImpl::invalidParameterHandler(
	const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved)
{
	Q_UNUSED(expression);
	Q_UNUSED(function);
	Q_UNUSED(file);
	Q_UNUSED(line);
	Q_UNUSED(pReserved);

	s_self->errorLogMessage(
		Common::PipeMessage::Crash,
		std::hash<std::thread::id>{}(std::this_thread::get_id()),
		__LINE__,
		__FILENAME__,
		__FUNCTION__,
		"invalid parameter handler"
	);

	s_self->setProcessSignaledState();
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

}
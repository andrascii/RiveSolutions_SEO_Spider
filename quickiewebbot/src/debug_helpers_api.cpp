#include "debug_helpers_api.h"

namespace QuickieWebBot
{

class MessageBoxThread : public QThread
{
public:
	MessageBoxThread(const std::string& text)
		: m_text(text)
		, m_result(IDRETRY)
	{
	}

	int askUser()
	{
		DEBUG_ASSERT(!isRunning());
		QThread::start();
		QThread::wait();
		return m_result;
	}

protected:

	virtual void run() override
	{
		m_result = MessageBoxA(0, m_text.c_str(), "QuickieWebBot Debug Library!",
#if defined(QT_DEBUG)
			MB_RETRYCANCEL
#else
			MB_OK
#endif
			| MB_ICONSTOP | MB_TOPMOST);
	}

private:
	int m_result;
	const std::string m_text;
};

void DebugHelpersApi::doAssert(const char* file, int line, const char* function, const char* expression)
{
	debugReport(file, line, function, expression);
}

void DebugHelpersApi::debugReport(const char* file, int line, const char* function, const char* expression)
{
	std::stringstream text;

	text << std::string("Debug Assertion") << std::endl << std::endl;
	text << std::string("Thread: ") << (ptrdiff_t)QThread::currentThreadId() << std::endl;
	text << std::string("Function: ") << function << std::endl;
	text << std::string("File: ") << file << std::endl;
	text << std::string("Line: ") << line << std::endl << std::endl;
	text << std::string("Expression: ") << expression << std::endl << std::endl;
	text << std::string("Press Retry to debug the application");

	MessageBoxThread msgbox(text.str());
	// Run message box in background thread to stop main thread message pump

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

void DebugHelpersApi::debugBreak()
{
#ifdef DEBUG

#ifdef Q_OS_WIN
	__debugbreak();
#else
	char* p = nullptr;
	*p = 0;
#endif

#endif
}

}
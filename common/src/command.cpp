#include "command.h"

namespace Common
{

void Command::setAssertData(const char* file, int line, const char* function, const void* thisptr,
	const char* condition, void* exceptionInfo, int exceptionSize)
{
	type = Command::Assert;
	AssertData* d = assertData();
	std::strncpy(d->file, file, _countof(d->file));
	d->line = line;
	std::strncpy(d->function, function, _countof(d->function));
	d->thisptr = thisptr;
	std::strncpy(d->condition, condition, _countof(d->condition));
	d->processId = static_cast<quintptr>(QCoreApplication::applicationPid());
	d->threadId = QThread::currentThreadId();
	d->exceptionInfo = exceptionInfo;
	d->exceptionSize = exceptionSize;
}

void Command::setDumpData(const void* exceptionInfo, int exceptionSize, int params)
{
	type = Command::Dump;
	DumpData* d = dumpData();
	d->processId = static_cast<quintptr>(QCoreApplication::applicationPid());
	d->threadId = QThread::currentThreadId();
	d->exceptionSize = exceptionSize;
	d->exceptionInfo = exceptionInfo;
	d->dumpParams = params;
}

void Command::setLogData(const char* file, int line, const char* function, const void* thisptr)
{
	type = Command::Log;
	LogData* d = logData();
	std::strncpy(d->file, file, _countof(d->file));
	d->line = line;
	std::strncpy(d->function, function, _countof(d->function));
	d->thisptr = thisptr;
	d->threadId = QThread::currentThreadId();
}

void Command::setCategoryData(const char* name)
{
	type = LogCategory;
	CategoryData* d = categoryData();
	std::strncpy(d->categoryName, name, _countof(d->categoryName));
}

void LogData::setText(LogData* logData, const char* text)
{
	std::strncpy(logData->text, text, std::strlen(text) + 1);
}

void Command::setRestartData(const char* displayMessage, bool showRestartDialog, int timeout, size_t argc, const char** argv)
{
	type = Restart;
	RestartData* d = restartData();
	std::strncpy(d->message, displayMessage, _countof(d->message));
	char* bufferPtr = d->cmdlineBuffer;
	*bufferPtr = 0;

	for (size_t i = 1; i < qMin<size_t>(argc, 256); ++i)
	{
		d->cmdlineTokens[i - 1] = bufferPtr - d->cmdlineBuffer;
		const size_t argLen1 = std::strlen(argv[i]) + 1;
		const size_t bufferTail = _countof(d->cmdlineBuffer) - (bufferPtr - d->cmdlineBuffer);
		std::strncpy(bufferPtr, argv[i], qMin(bufferTail, argLen1));

		bufferPtr += argLen1;
	}

	d->cmdlineTokens[qMin<size_t>(argc - 1, 255)] = -1;
	d->showDialog = showRestartDialog;
	d->timeout = timeout;
}

}
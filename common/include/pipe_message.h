#pragma once

namespace Common
{

constexpr int c_fileNameLength = 256;
constexpr int c_functionNameLength = 256;
constexpr int c_messageLength = 4096;

enum class SeverityLevel
{
	TraceLevel,
	DebugLevel,
	InfoLevel,
	WarningLevel,
	ErrorLevel
};

#pragma pack(push, 1)

struct PipeMessage
{
	enum Type : std::uint64_t
	{
		Assert,
		Crash,
		LogMessage
	};

	Type type;
	std::uint64_t severityLevel;
	std::uint64_t threadId;
	std::uint32_t line;
	char file[c_fileNameLength];
	char function[c_functionNameLength];
	char message[c_messageLength];
};

#pragma pack(pop)

}

Q_DECLARE_METATYPE(Common::SeverityLevel)
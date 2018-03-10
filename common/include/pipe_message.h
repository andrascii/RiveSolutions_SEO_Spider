#pragma once

namespace Common
{

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
	char file[256];
	char function[256];
	char message[4096];
};

#pragma pack(pop)

}

Q_DECLARE_METATYPE(Common::SeverityLevel)
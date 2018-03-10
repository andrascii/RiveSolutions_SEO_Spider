#pragma once

namespace Common
{

#pragma pack(push, 1)

struct PipeMessage
{
	std::uint64_t severityLevel;
	char message[4096];
};

#pragma pack(pop)

}
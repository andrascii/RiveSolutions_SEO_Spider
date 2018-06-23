#pragma once

namespace Common
{

constexpr int c_dataLength = 4096;

enum class CommandType
{
	CommandTypeTakeScreenshot,
	CommandTypeExit
};

struct Command
{
	CommandType type;
	char data[c_dataLength];
};

}
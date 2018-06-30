#pragma once

namespace Common
{

constexpr int c_dataLength = 4096;

enum class ScreenshotCommandType
{
	CommandTypeTakeScreenshot,
	CommandTypeExit
};

struct ScreenshotCommand
{
	ScreenshotCommandType type;
	char data[c_dataLength];
};

}
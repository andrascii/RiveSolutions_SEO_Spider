#pragma once
#include "stdafx.h"

namespace Common
{

#undef max

struct ScreenshotMakerExit
{
};

struct ScreenshotMakerResponse
{
	bool operationComplete;
};

struct ScreenshotMakerTakeScreenshot
{
	static constexpr int c_dataLength = 4096;

	char data[c_dataLength];
};

struct ScreenshotMakerMessage
{
	enum Type
	{
		TypeTakeScreenshot,
		TypeExit,
		TypeScreenshotMakerResponse
	};

	const ScreenshotMakerTakeScreenshot* takeScreenshotMessage() const;
	ScreenshotMakerTakeScreenshot* takeScreenshotMessage();

	const ScreenshotMakerResponse* responseMessage() const;
	ScreenshotMakerResponse* responseMessage();

	void setTakeScreenshotMessage(const QUrl& url);
	void setResponse(bool result);
	void setExitMessage();

	constexpr static std::size_t s_messageSize = std::max(
		std::max(sizeof(ScreenshotMakerExit), sizeof(ScreenshotMakerTakeScreenshot)),
		sizeof(ScreenshotMakerResponse));

	Type type;
	char data[s_messageSize];
};

}
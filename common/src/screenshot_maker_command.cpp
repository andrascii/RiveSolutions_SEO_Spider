#ifdef ENABLE_SCREENSHOTS
#include "screenshot_maker_command.h"

namespace Common
{

const ScreenshotMakerTakeScreenshot* ScreenshotMakerMessage::takeScreenshotMessage() const
{
	return type == TypeTakeScreenshot ?
		reinterpret_cast<const ScreenshotMakerTakeScreenshot*>(&data) : nullptr;
}

ScreenshotMakerTakeScreenshot* ScreenshotMakerMessage::takeScreenshotMessage()
{
	return const_cast<ScreenshotMakerTakeScreenshot*>(const_cast<const ScreenshotMakerMessage*>(this)->takeScreenshotMessage());
}

const ScreenshotMakerResponse* ScreenshotMakerMessage::responseMessage() const
{
	return type == TypeScreenshotMakerResponse ?
		reinterpret_cast<const ScreenshotMakerResponse*>(&data) : nullptr;
}

ScreenshotMakerResponse* ScreenshotMakerMessage::responseMessage()
{
	return const_cast<ScreenshotMakerResponse*>(const_cast<const ScreenshotMakerMessage*>(this)->responseMessage());
}

void ScreenshotMakerMessage::setTakeScreenshotMessage(const QUrl& url)
{
	type = TypeTakeScreenshot;
	ScreenshotMakerTakeScreenshot* takeScreenshotMsg = takeScreenshotMessage();
	const std::string& urlData = url.toDisplayString().toStdString();
	std::strcpy(takeScreenshotMsg->data, urlData.data());
	//std::copy(urlData.begin(), urlData.end(), takeScreenshotMsg->data);
}

void ScreenshotMakerMessage::setResponse(bool result)
{
	type = TypeScreenshotMakerResponse;
	ScreenshotMakerResponse* response = responseMessage();
	response->operationComplete = result;
}

void ScreenshotMakerMessage::setExitMessage()
{
	type = TypeExit;
}

}

#endif
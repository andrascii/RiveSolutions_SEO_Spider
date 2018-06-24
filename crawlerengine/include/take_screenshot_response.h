#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

class ITakeScreenshotResponse : public IResponse
{
public:
	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTakeScreenshot)

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTakeScreenshot;
	}

	virtual QVariant screenshot() const = 0;
};

class TakeScreenshotResponse : public ITakeScreenshotResponse
{
public:
	TakeScreenshotResponse()
	{
	}

	TakeScreenshotResponse(const QPixmap& screenshot)
		: m_screenshot(screenshot)
	{
	}

	virtual QVariant screenshot() const override
	{
		return m_screenshot;
	}

private:
	QPixmap m_screenshot;
};

class TakeScreenshotResponseTest : public ITakeScreenshotResponse
{
public:
	virtual QVariant screenshot() const override
	{
		return m_screenshot;
	}

private:
	QImage m_screenshot;
};

}

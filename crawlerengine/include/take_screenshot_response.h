#pragma once
#include "stdafx.h"

#include "iresponse.h"

namespace CrawlerEngine
{

class ITakeScreenshotResponse : public IResponse
{
public:
	enum Error
	{
		ErrorNoError,
		ErrorCannotStartScreenshotMakerProcess,
		ErrorCannotEstablishConnection
	};

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTakeScreenshot)

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTakeScreenshot;
	}

	virtual Error error() const = 0;
	virtual QVariant screenshot() const = 0;
};

class TakeScreenshotResponse : public ITakeScreenshotResponse
{
public:
	TakeScreenshotResponse()
		: m_error(ErrorNoError)
	{
	}

	TakeScreenshotResponse(const QPixmap& screenshot)
		: m_screenshot(screenshot)
		, m_error(ErrorNoError)
	{
	}

	void setError(Error error)
	{
		m_error = error;
	}

	virtual Error error() const override
	{
		return m_error;
	}

	virtual QVariant screenshot() const override
	{
		return m_screenshot;
	}

private:
	QPixmap m_screenshot;
	Error m_error;
};

class TakeScreenshotResponseTest : public ITakeScreenshotResponse
{
public:
	virtual Error error() const override
	{
		return ErrorNoError;
	}

	virtual QVariant screenshot() const override
	{
		return m_screenshot;
	}

private:
	QImage m_screenshot;
};

}

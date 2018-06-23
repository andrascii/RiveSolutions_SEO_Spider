#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

class TakeScreenshotResponse : public IResponse
{
public:
	TakeScreenshotResponse(const QPixmap& pixmap)
		: m_pixmap(pixmap)
	{
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTakeScreenshot)

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTakeScreenshot;
	}

	const QPixmap& pixmap() const
	{
		return m_pixmap;
	}

private:
	QPixmap m_pixmap;
};

}

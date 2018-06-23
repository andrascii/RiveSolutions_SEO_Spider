#pragma once

#include "irequest.h"
#include "url.h"

namespace CrawlerEngine
{

class TakeScreenshotRequest : public IRequest
{
public:
	TakeScreenshotRequest(const Url& pageUrl)
		: m_pageUrl(pageUrl)
	{
	}

	virtual IRequest* clone() const override
	{
		return new TakeScreenshotRequest(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestTakeScreenshot;
	}

	const Url& url() const
	{
		return m_pageUrl;
	}

private:
	Url m_pageUrl;
};

}
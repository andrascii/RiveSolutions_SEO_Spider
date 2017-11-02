#pragma once

#include "iresponse.h"

namespace CrawlerEngine
{

	struct DownloadResponse : public Common::IResponse
	{
		virtual Common::ResponseType type() const noexcept override
		{
			return Common::ResponseType::ResponseTypeDownload;
		}

		DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(Common::ResponseType::ResponseTypeDownload)

		int statusCode;
		QUrl url;
		QUrl redirectUrl;
		QByteArray responseBody;
		QByteArray responseHeaderValuePairs;
		QList<QPair<QByteArray, QByteArray>> responseHeaders;
	};

}
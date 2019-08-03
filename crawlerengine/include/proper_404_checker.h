#pragma once
#include "stdafx.h"

#include "requester_wrapper.h"
#include "url.h"

namespace CrawlerEngine
{
struct DownloadResponse;

struct Check404IsProperRequest : IRequest
{
	Check404IsProperRequest(const Url& url)
		: url(url)
	{
	}

	virtual std::shared_ptr<IRequest> clone() const override
	{
		return std::make_shared<Check404IsProperRequest>(*this);
	}
	virtual RequestType requestType() const noexcept override
	{
		return RequestType::RequestCheck404IsProper;
	}

	Url url;
};


struct Check404IsProperResponse : public IResponse
{
	Check404IsProperResponse(bool result)
		: result(result)
	{
	}

	virtual ResponseType type() const noexcept override
	{
		return ResponseType::ResponseTypeCheck404IsProper;
	}

	DEFINE_RESPONSE_STATIC_TYPE_IN_CLASS(ResponseType::ResponseTypeCheck404IsProper)

	bool result;
};


class Proper404Checker : public QObject
{
	Q_OBJECT

public:
	Proper404Checker();
	~Proper404Checker();

	Q_INVOKABLE void handleRequest(RequesterSharedPtr requester);
	Q_INVOKABLE void stopRequestHandling(RequesterSharedPtr requester);
	QObject* qobject();

private:
	void onLoadingDone(Requester* requester, const DownloadResponse& response);


private:
	RequesterSharedPtr m_requester;
	RequesterWrapper m_downloadRequester;
};

}

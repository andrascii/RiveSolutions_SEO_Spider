#pragma once

#include "parsed_page.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

class WorkerResult final
{
public:
	WorkerResult();
	WorkerResult(ParsedPagePtr incomingPage, bool refresh, DownloadRequestType downloadRequestType);

	ParsedPagePtr& incomingPage() noexcept;
	const ParsedPagePtr& incomingPageConstRef() const noexcept;
	bool isRefreshResult() const noexcept;
	DownloadRequestType requestType() const noexcept;

private:
	ParsedPagePtr m_incomingPage;
	bool m_refresh;
	DownloadRequestType m_requestType;
};

Q_DECLARE_METATYPE(WorkerResult)

}
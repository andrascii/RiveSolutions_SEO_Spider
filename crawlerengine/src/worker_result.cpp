#include "worker_result.h"

namespace CrawlerEngine
{

WorkerResult::WorkerResult()
{
	ASSERT(!"This ctor added only for conformance with Qt Meta System");
}

WorkerResult::WorkerResult(ParsedPagePtr incomingPage, bool refresh, DownloadRequestType downloadRequestType)
	: m_incomingPage(incomingPage)
	, m_refresh(refresh)
	, m_requestType(downloadRequestType)
{
}

ParsedPagePtr& WorkerResult::incomingPage() noexcept
{
	return m_incomingPage;
}

const ParsedPagePtr& WorkerResult::incomingPageConstRef() const noexcept
{
	return m_incomingPage;
}

bool WorkerResult::isRefreshResult() const noexcept
{
	return m_refresh;
}

DownloadRequestType WorkerResult::requestType() const noexcept
{
	return m_requestType;
}
}

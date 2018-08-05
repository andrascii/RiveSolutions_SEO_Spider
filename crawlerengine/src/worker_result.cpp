#include "worker_result.h"

namespace CrawlerEngine
{

WorkerResult::WorkerResult()
{
	ASSERT(!"This ctor added only for conformance with Qt Meta System");
}

WorkerResult::WorkerResult(ParsedPagePtr incomingPage,
	bool refresh,
	DownloadRequestType downloadRequestType,
	const std::vector<bool>& storagesBeforeRemoving)
	: m_incomingPage(incomingPage)
	, m_refresh(refresh)
	, m_requestType(downloadRequestType)
	, m_storagesBeforeRemoving(storagesBeforeRemoving)
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

const std::vector<bool>& WorkerResult::storagesBeforeRemoving() const
{
	return m_storagesBeforeRemoving;
}

}

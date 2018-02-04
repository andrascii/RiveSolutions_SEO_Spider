#include "worker_result.h"

namespace CrawlerEngine
{

WorkerResult::WorkerResult()
{
	ASSERT(!"This ctor added only for conformance with Qt Meta System");
}

WorkerResult::WorkerResult(ParsedPagePtr incomingPage, bool refresh)
	: m_incomingPage(incomingPage)
	, m_refresh(refresh)
{
}

ParsedPagePtr& WorkerResult::incomingPage() noexcept
{
	return m_incomingPage;
}

bool WorkerResult::isRefreshResult() const noexcept
{
	return m_refresh;
}

}
#pragma once

#include "parsed_page.h"

namespace CrawlerEngine
{

class WorkerResult final
{
public:
	WorkerResult();
	WorkerResult(ParsedPagePtr incomingPage, bool refresh);

	ParsedPagePtr& incomingPage() noexcept;
	bool isRefreshResult() const noexcept;

private:
	ParsedPagePtr m_incomingPage;
	bool m_refresh;
};

Q_DECLARE_METATYPE(WorkerResult)

}
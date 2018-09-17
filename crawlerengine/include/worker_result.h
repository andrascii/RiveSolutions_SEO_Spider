#pragma once

#include "parsed_page.h"
#include "crawler_request.h"

namespace CrawlerEngine
{

class WorkerResult final
{
public:
	WorkerResult();

	WorkerResult(ParsedPagePtr incomingPage,
		bool refresh,
		DownloadRequestType downloadRequestType,
		const std::vector<bool>& storagesBeforeRemoving);

	ParsedPagePtr& incomingPage() noexcept;
	const ParsedPagePtr& incomingPageConstRef() const noexcept;
	bool isRefreshResult() const noexcept;
	DownloadRequestType requestType() const noexcept;
	const std::vector<bool>& storagesBeforeRemoving() const;

private:
	ParsedPagePtr m_incomingPage;
	bool m_refresh;
	DownloadRequestType m_requestType;
	std::vector<bool> m_storagesBeforeRemoving;
};

Q_DECLARE_METATYPE(WorkerResult)

}
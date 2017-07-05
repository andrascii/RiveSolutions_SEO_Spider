#pragma once

#include "crawler_page_info_acceptor.h"
#include "website_analyse_element.h"
#include "model_controller.h"

namespace QuickieWebBot
{

class CrawlerController : public QObject
{
	Q_OBJECT

public:
	enum State
	{
		CrawlerWorkingState,
		CrawlerPendingState
	};

	CrawlerController(unsigned int threadCount, QObject* parent = nullptr);

	Q_SIGNAL void addedNewElement(WebSiteAnalyseElementPtr element);

	Q_SLOT void start();
	Q_SLOT void stop();

	void setThreadCount(unsigned int threadCount) noexcept;

private:
	void prepareAndStart() noexcept;

	Q_SLOT void onElementAdded(QThread* fromThread, WebSiteAnalyseElementPtr element);

private:
	bool m_stop;
	int m_state;

	std::vector<QThread*> m_workers;
};

}

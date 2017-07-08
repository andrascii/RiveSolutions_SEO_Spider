#pragma once

#include "page_info.h"
#include "model_controller.h"

namespace QuickieWebBot
{

class CrawlerPageInfoAcceptor;

class Crawler : public QObject
{
	Q_OBJECT

private:
	struct QUrlHasher
	{
		size_t operator()(const QUrl& url) const noexcept
		{
			return hasher(url.toString().toStdString());
		}

		boost::hash<std::string> hasher;
	};

public:
	Crawler(unsigned int threadCount, QObject* parent = nullptr);
	~Crawler();

	Q_SIGNAL void addedPageInfo(PageInfoPtr element);

	Q_SLOT void start();
	Q_SLOT void stop();

	void setThreadCount(unsigned int threadCount) noexcept;

private:
	Q_SLOT void onPageInfoParsed(QThread* fromThread, PageInfoPtr pageInfo);

private:
	bool m_stop;
	bool m_initialCrawling;

	std::unordered_set<QUrl, QUrlHasher> m_internalUrlList;
	std::unordered_set<QUrl, QUrlHasher> m_crawledUrlList;
	std::map<QThread*, CrawlerPageInfoAcceptor*> m_workers;
};

}

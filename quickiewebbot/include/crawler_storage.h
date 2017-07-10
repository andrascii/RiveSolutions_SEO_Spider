#pragma once

#include "locking_stl_collections.h"

namespace QuickieWebBot
{

class CrawlerStorage
{
public:

	void setHost(const QUrl& url);

	//
	// Returns random url for crawling
	//
	QUrl get() noexcept;
	void saveUrlList(const std::vector<QUrl>& urlList) noexcept;

private:
	struct QUrlHasher
	{
		size_t operator()(const QUrl& url) const noexcept
		{
			return hasher(url.toString().toStdString());
		}

		boost::hash<std::string> hasher;
	};

	LockingUnorderedSet<QUrl, QUrlHasher> m_internalUrlList;
	LockingUnorderedSet<QUrl, QUrlHasher> m_crawledUrlList;

	std::condition_variable m_condition;
	std::mutex m_mutex;
};

}

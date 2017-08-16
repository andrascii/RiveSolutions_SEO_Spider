#pragma once

namespace WebCrawler
{

class WebCrawlerInternalUrlStorage
{
public:
	
	// all methods are thread-safe

	void setHost(const QUrl& url);

	// returns random url for crawling (async operation)
	// returns false if urls queue is empty
	// otherwise returns true and write url to passed argument
	bool extractUrl(QUrl& url) noexcept;

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

	std::unordered_set<QUrl, QUrlHasher> m_internalUrlList;
	std::unordered_set<QUrl, QUrlHasher> m_crawledUrlList;

	std::mutex m_mutex;
};

}

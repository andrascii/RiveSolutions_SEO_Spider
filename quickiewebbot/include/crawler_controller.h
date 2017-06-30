#pragma once

namespace QuickieWebBot
{

class CrawlerController : public QObject
{
	Q_OBJECT

public:
	CrawlerController();

	void startCrawlers(size_t num);
	void stopCrawlers();

private:
	std::vector<QThread*> m_workerThreads;
};

}

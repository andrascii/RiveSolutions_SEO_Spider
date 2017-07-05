#pragma once

namespace QuickieWebBot
{

class CrawlerController : public QObject
{
	Q_OBJECT

public:
	CrawlerController();

private:
	std::vector<QThread*> m_workerThreads;
};

}

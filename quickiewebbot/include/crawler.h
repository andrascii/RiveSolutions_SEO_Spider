#pragma once

#include "page_info.h"
#include "crawler_storage.h"

namespace QuickieWebBot
{

class PageInfoAcceptor;
class ModelController;

class Crawler : public QObject
{
	Q_OBJECT

public:
	Crawler(unsigned int threadCount, ModelController* modelController, QObject* parent = nullptr);
	~Crawler();

	Q_SLOT void start();
	Q_SLOT void stop();

private:
	Q_SLOT void onPageInfoParsed(PageInfoPtr pageInfo);

private:
	ModelController* m_modelController;
	CrawlerStorage m_storage;
	std::vector<std::pair<QThread*, PageInfoAcceptor*>> m_workers;
};

}

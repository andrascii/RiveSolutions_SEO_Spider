#pragma once

#include "crawler.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

class CrawlerStatusInfo : public QFrame
{
	Q_OBJECT

public:
	CrawlerStatusInfo(QWidget* parent);

private slots:
	void onAboutCrawlerProgressChanged(CrawlingProgress progress);

private:
	QLabel* m_crawledLinksLabel;
	QLabel* m_pendingLinksLabel;
};

}
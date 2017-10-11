#pragma once

#include "crawler.h"

namespace QuickieWebBot
{

using namespace WebCrawler;

class CrawlerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	CrawlerProgressBar(QWidget* parent = nullptr);

private slots:
	void calculatePercents(CrawlingState state);
};

}

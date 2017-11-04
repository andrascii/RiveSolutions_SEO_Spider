#pragma once

#include "crawler.h"

namespace SeoSpider
{

using namespace CrawlerEngine;

class CrawlerProgressBar : public QProgressBar
{
	Q_OBJECT

public:
	CrawlerProgressBar(QWidget* parent = nullptr);

private slots:
	void calculatePercents(CrawlingProgress progress);
};

}

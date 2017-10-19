#pragma once

#include "irobots_txt_loader.h"

namespace WebCrawlerTests
{

class TestsRobotsTxtLoader: public QObject, public WebCrawler::IRobotsTxtLoader
{
	Q_OBJECT
public:
	virtual void load(const QUrl& url) override;
	Q_SIGNAL virtual void ready(const QByteArray& content) override;
	virtual QObject* qObject() override;
};

}
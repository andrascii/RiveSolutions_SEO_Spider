#pragma once

#include "irobots_txt_loader.h"

namespace WebCrawlerTests
{

class TestsRobotsTxtLoader: public QObject, public WebCrawler::IRobotsTxtLoader
{
	Q_OBJECT

public:
	virtual void setUserAgent(const QByteArray& userAgent) override;

	virtual void load(const QUrl& url) override;
	virtual const QByteArray& content() const noexcept override;
	virtual bool isReady() const noexcept override;

	virtual QObject* qobject() override;

signals:
	virtual void ready() override;

private:
	QByteArray m_content;
};

}
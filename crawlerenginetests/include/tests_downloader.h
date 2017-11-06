#pragma once

#include "idownloader.h"

namespace CrawlerEngine
{

struct CrawlerRequest;

}

namespace CrawlerEngineTests
{

class TestsDownloader : public QObject, public CrawlerEngine::IDownloader
{
	Q_OBJECT

public:
	TestsDownloader();

	~TestsDownloader();

	virtual void setUserAgent(const QByteArray& userAgent) override;

	Q_INVOKABLE virtual void handleRequest(CrawlerEngine::RequesterSharedPtr requester) override;

	Q_INVOKABLE virtual void stopRequestHandling(CrawlerEngine::RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private:
	QDir testsDataDir() const;

	std::pair<QString, QString> mapUrlToTestDataFiles(const CrawlerEngine::CrawlerRequest& url) const;

private:
	mutable QString m_testDataPath;
};

}
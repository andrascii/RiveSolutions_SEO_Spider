#pragma once

#include "idownloader.h"
#include "hops_chain.h"

namespace CrawlerEngine
{

struct CrawlerRequest;
struct DownloadResponse;
struct DownloadRequest;

}

namespace CrawlerEngineTests
{

class TestsDownloader : public QObject, public CrawlerEngine::IDownloader
{
	Q_OBJECT

public:
	using PostProcessorFunc = std::function<void(CrawlerEngine::DownloadResponse&)>;

	TestsDownloader();
	~TestsDownloader();

	Q_INVOKABLE virtual void setPauseRange(int from, int to) override;
	Q_INVOKABLE virtual void resetPauseRange() override;
	Q_INVOKABLE virtual void setTimeout(int msecs) override;
	Q_INVOKABLE virtual void setMaxRedirects(int redirects) override;
	Q_INVOKABLE virtual void setUserAgent(const QByteArray& userAgent) override;
	Q_INVOKABLE virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	Q_INVOKABLE virtual void resetProxy() override;
	Q_INVOKABLE virtual void handleRequest(CrawlerEngine::RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(CrawlerEngine::RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;
	void setPostProcessor(PostProcessorFunc postProcessor);

private:
	HopsChain hopsChain(const DownloadRequest& request, QSet<QString>& uniqueUrls) const;
	QDir testsDataDir() const;
	std::pair<QString, QString> mapUrlToTestDataFiles(const DownloadRequest& downloadRequest) const;

private:
	mutable QString m_testDataPath;
	PostProcessorFunc m_responsePostProcessor;
};

}
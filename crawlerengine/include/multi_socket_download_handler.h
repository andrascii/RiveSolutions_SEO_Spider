#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "idownloader.h"

namespace CrawlerEngine
{

class ResponseHeaders;
class RandomIntervalRangeTimer;
class MultiSocketLoader;
struct DownloadResponse;

class MultiSocketDownloadHandler : public QObject, public IDownloader
{
	Q_OBJECT

public:
	MultiSocketDownloadHandler();

	Q_INVOKABLE virtual void setPauseRange(int from, int to) override;
	Q_INVOKABLE virtual void resetPauseRange() override;
	Q_INVOKABLE virtual void setTimeout(int msecs) override;
	Q_INVOKABLE virtual void setMaxRedirects(int redirects) override;
	Q_INVOKABLE virtual void setUserAgent(const QByteArray& userAgent) override;
	Q_INVOKABLE virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	Q_INVOKABLE virtual void resetProxy() override;
	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private slots:
	void onTimerTicked();
	void onAboutDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
	bool isAutoDetectionBodyProcessing(int requestId) const;

	void onUrlLoaded(int id,
		const QByteArray& url,
		const QByteArray& data,
		const ResponseHeaders& responseHeaders,
		Common::StatusCode statusCode,
		int timeElapsed);

private:
	void load(RequesterSharedPtr requester);
	int maxRedirectsToProcess() const noexcept;
	int loadHelper(const CrawlerRequest& request);
	void proxyAuthenticationRequired() const;
	std::shared_ptr<DownloadResponse> responseFor(int requestId);
	Url redirectedUrl(const ResponseHeaders& responseHeaders) const;

	RequesterSharedPtr requesterById(int id) const;
	int parentIdFor(int id) const;

	void followLocation(const std::shared_ptr<DownloadResponse>& response,
		int parentRequestId,
		const Url& url,
		const Url& redirectUrlAddress,
		const QByteArray& data,
		DownloadRequestType requestType,
		Common::StatusCode statusCode,
		const ResponseHeaders& responseHeaders,
		int timeElapsed);

private:
	struct RequestIdBindings
	{
		QMap<int, int> redirectRequestIdToParentId;
		QMap<int, QVector<int>> parentIdToRedirectRequestId;
	};

	MultiSocketLoader* m_multiSocketLoader;
	QMap<int, RequesterWeakPtr> m_requesters;
	QVector<int> m_activeRequests;
	QMap<int, std::shared_ptr<DownloadResponse>> m_responses;
	QByteArray m_userAgent;
	RandomIntervalRangeTimer* m_randomIntervalRangeTimer;
	int m_timeout;
	int m_maxRedirects;
	std::deque<RequesterSharedPtr> m_requesterQueue;
	RequestIdBindings m_idBindings;
};

}

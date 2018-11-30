#pragma once

#include "requester.h"
#include "crawler_request.h"
#include "abstract_download_handler.h"
#include "download_request.h"

namespace CrawlerEngine
{

class ResponseHeaders;
class RandomIntervalRangeTimer;
class MultiSocketLoader;
struct DownloadResponse;

class MultiSocketDownloadHandler : public AbstractDownloadHandler
{
	Q_OBJECT

public:
	MultiSocketDownloadHandler();

	Q_INVOKABLE virtual void setUserAgent(const QByteArray& userAgent) override;
	Q_INVOKABLE virtual void setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword) override;
	Q_INVOKABLE virtual void resetProxy() override;
	Q_INVOKABLE virtual void setTimeout(int msecs) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;

	virtual QObject* qobject() override;

private slots:
	void onAboutDownloadProgress(int id, double bytesTotal, double bytesReceived);
	void onAboutUploadProgress(int id, double bytesTotal, double bytesSent);

	void onUrlLoaded(int id,
		const QByteArray& url,
		const QByteArray& data,
		const ResponseHeaders& responseHeaders,
		Common::StatusCode statusCode,
		int timeElapsed);

private:
	virtual void load(RequesterSharedPtr requester) override;
	virtual std::shared_ptr<DownloadResponse> responseFor(int requestId) override;

private:
	int loadHelper(const CrawlerRequest& request, DownloadRequest::BodyProcessingCommand bodyProcessingCommand);
	Url redirectedUrl(const ResponseHeaders& responseHeaders, const Url& baseAddress) const;

	RequesterSharedPtr requesterByIdAssertIfNotExists(int id) const;
	RequesterSharedPtr requesterById(int id) const;
	int parentIdFor(int id) const;

	void followLocation(DownloadRequest::BodyProcessingCommand bodyProcessingCommand,
		const std::shared_ptr<DownloadResponse>& response,
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
	RequestIdBindings m_idBindings;
};

}

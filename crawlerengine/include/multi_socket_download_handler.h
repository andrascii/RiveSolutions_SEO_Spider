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
class HopsChain;
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

	void onCurrentParallelTransfersCountChanged(int count);

private:
	virtual void load(RequesterSharedPtr requester) override;
	virtual std::shared_ptr<DownloadResponse> responseFor(int requestId) override;
	virtual void pauseRequesters(const QList<Requester*>& requesterToBePaused) override;
	virtual void unpauseRequesters(const QList<Requester*>& requesterToBeUnpaused) override;

private:
	int loadHelper(const CrawlerRequest& request, DownloadRequest::BodyProcessingCommand bodyProcessingCommand);

	//! extracts and returns the value of the Location header (resolves the location address by baseAddress)
	Url redirectedUrl(const ResponseHeaders& responseHeaders, const Url& baseAddress) const;

	//! Returns the identifier of the active requester by loaded resource identifier.
	//! Search performs with respect of redirections.
	//! Returns -1 if requester was not found.
	int actualRequesterIdByLoadedResourceId(int id) const;

	RequesterSharedPtr requesterByIdAssertIfNotExists(int id) const;
	RequesterSharedPtr requesterById(int id) const;
	int parentIdFor(int id) const;

	void removeRequestIndexesChain(int id);

	//! creates new request to load a redirection target
	void followLocation(DownloadRequest::BodyProcessingCommand bodyProcessingCommand,
		int parentRequestId,
		const Url& redirectUrlAddress,
		DownloadRequestType requestType);

	//! true if redirectUrlAddress contains twice in the hopsChain
	bool isRedirectLoop(const HopsChain& hopsChain, const Url& redirectUrlAddress) const;

	//! returns the valid request indexes which should be paused
	QVector<int> requestIndexesToPause() const;

	//! returns the valid request indexes which should be unpaused and stored in the m_activeRequesters and m_pendingRequesters
	QVector<int> requestIndexesToUnpause(const QList<Requester*>& requesterToBeUnpaused) const;

	//! returns first unpaused requester and also clears expired requesters by searching pass
	RequesterSharedPtr extractFirstUnpausedRequester();

	//! removes all request binded data with passed identifier
	void removeLoadedResourceAssociatedData(int id, Requester* requester);

private:
	MultiSocketLoader* m_multiSocketLoader;

	//! contains mapping redirect request indexes to their parent request indexes
	//! { redirection request index => parent request index }
	//! where parent request id for redirection request id can also be the redirection request id
	QMap<int, int> m_redirectRequestIdToParentId;

	//! contains { parent request id (first request) => list of redirection requests ids }
	QMap<int, QVector<int>> m_parentIdToRedirectIds;

	//! current active requesters (which are already passed to the libcurl)
	QMap<int, RequesterWeakPtr> m_activeRequesters;

	//! responses for the active requesters
	QMap<int, std::shared_ptr<DownloadResponse>> m_responses;

	//! requesters will be added to this container
	//! if maximum parallel transfer count achieved
	QQueue<RequesterWeakPtr> m_pendingRequesters;

	//! stores the pointers to the requesters which must be paused
	QSet<Requester*> m_pausedRequesters;
};

}

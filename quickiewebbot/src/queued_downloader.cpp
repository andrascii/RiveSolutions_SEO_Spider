#include "queued_downloader.h"
#include "constants.h"

namespace QuickieWebBot
{

QueuedDownloader::QueuedDownloader()
	: AbstractThreadableObject(this, QByteArray("QuickieWebBot::QueuedDownloaderThread"))
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_unprocessedRepliesCount(0)
	, m_pendingReguestsCount(0)
{
	moveThisToSeparateThread();

	VERIFY(connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*))));
}

QueuedDownloader::~QueuedDownloader()
{
	stopExecution();
}

void QueuedDownloader::scheduleUrl(const QUrl& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);
	m_requestQueue.push_back(url);
}

bool QueuedDownloader::extractReply(Reply& response) noexcept
{
	std::unique_lock<std::mutex> locker(m_repliesQueueMutex); 
		
	if (m_repliesQueue.empty())
	{
		return false;
	}

	response = std::move(*m_repliesQueue.begin());
	m_repliesQueue.erase(m_repliesQueue.begin());
	m_unprocessedRepliesCount--;

	return true;
}

void QueuedDownloader::urlDownloaded(QNetworkReply* reply)
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	Reply response{ statusCode.isValid() ? statusCode.toInt() : -1, reply->url(), reply->readAll() };

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		response.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
	}

	m_repliesQueue.push_back(std::move(response));
	m_pendingReguestsCount--;
	m_unprocessedRepliesCount++;
}

void QueuedDownloader::process()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	static const int s_maxUnprocessedReplies = 200;
	static const int s_maxPendingRequests = 50;
	static const int s_maxRequestsOneBatch = 100;

	int requestsThisBatch = 0;
	
	while (m_requestQueue.size() && requestsThisBatch < s_maxRequestsOneBatch &&
		m_pendingReguestsCount < s_maxPendingRequests && m_unprocessedRepliesCount < s_maxUnprocessedReplies)
	{
		m_networkAccessManager->get(QNetworkRequest(*m_requestQueue.begin()));
		m_requestQueue.erase(m_requestQueue.begin());

		m_pendingReguestsCount++;
		requestsThisBatch++;
	}
}

}
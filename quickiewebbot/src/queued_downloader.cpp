#include "queued_downloader.h"
#include "constants.h"

namespace QuickieWebBot
{

QueuedDownloader::QueuedDownloader()
	: AbstractThreadableObject(this)
	, m_networkAccessManager(new QNetworkAccessManager(this))
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

void QueuedDownloader::scheduleUrlList(const QList<QUrl>& urlList) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);
	m_requestQueue.insert(m_requestQueue.end(), urlList.begin(), urlList.end());
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

	return true;
}

void QueuedDownloader::urlDownloaded(QNetworkReply* reply)
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	Reply response{ reply->url(), reply->readAll() };

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		response.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
	}

	m_repliesQueue.push_back(std::move(response));
}

void QueuedDownloader::process()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	foreach(const QUrl& url, m_requestQueue)
	{
		m_networkAccessManager->get(QNetworkRequest(url));
	}

	m_requestQueue.clear();
}

}
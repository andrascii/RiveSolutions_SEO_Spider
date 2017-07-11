#include "queued_downloader.h"
#include "constants.h"

namespace QuickieWebBot
{

QueuedDownloader::QueuedDownloader()
	: m_networkAccessManager(new QNetworkAccessManager(this))
{
	moveThisToSeparateThread();

	VERIFY(connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*))));
}

QueuedDownloader::~QueuedDownloader()
{
	stop();
}

void QueuedDownloader::stop()
{
	AbstractThreadableObject::stop();
	m_repliesWaitCondition.notify_all();
}

void QueuedDownloader::scheduleUrl(const QUrl& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);
	m_requestQueue.push_back(url);
}

void QueuedDownloader::scheduleUrlList(const QList<QUrl>& urlList) noexcept
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);
	m_requestQueue.append(urlList);
}

bool QueuedDownloader::extractReply(Reply& response, ReplyExtractPolicy type) noexcept
{
	if (type == SuspendExtractPolicy)
	{
		std::unique_lock<std::mutex> locker(m_repliesQueueMutex);

		while (!m_repliesQueue.size())
		{
			m_repliesWaitCondition.wait(locker);

			if (!isRunning())
			{
				return false;
			}
		}

		response = std::move(*m_repliesQueue.begin());
		m_repliesQueue.erase(m_repliesQueue.begin());

		return true;
	}

	if (type == AsyncExtractPolicy)
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

	assert(!"Invalid ReplyExtractPolicy");
	return false;
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

	m_repliesWaitCondition.notify_all();
}

void QueuedDownloader::process()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	foreach(const QUrl& url, m_requestQueue)
	{
		m_networkAccessManager->get(QNetworkRequest(url));
		m_requestQueue.erase(m_requestQueue.begin());
	}
}

}
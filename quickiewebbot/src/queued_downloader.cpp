#include "queued_downloader.h"
#include "constants.h"

namespace QuickieWebBot
{

QueuedDownloader::QueuedDownloader()
	: QObject(nullptr)
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_timerId(0)
	, m_isRunning(false)
{
	moveToThread(&m_thisThread);

	VERIFY(connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*))));
}

QueuedDownloader::~QueuedDownloader()
{
	stop();
	wait();
}

bool QueuedDownloader::isRunning() const noexcept
{
	return m_isRunning.load();
}

void QueuedDownloader::start()
{
	if (!m_thisThread.isRunning())
	{
		m_thisThread.start();

		QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
			Q_ARG(int, Common::g_minimumRecommendedTimerResolution), Q_ARG(Qt::TimerType, Qt::CoarseTimer));

		assert(m_timerId);

		m_isRunning.store(true);
	}
}

void QueuedDownloader::stop()
{
	if (m_thisThread.isRunning())
	{
		assert(m_timerId);

		QMetaObject::invokeMethod(this, "killTimer",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_timerId));

		m_thisThread.quit();

		m_isRunning.store(false);
		m_repliesWaitCondition.notify_all();
	}
}

void QueuedDownloader::wait()
{
	m_thisThread.wait();
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

void QueuedDownloader::timerEvent(QTimerEvent* event)
{
	processUrlQueue();
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

void QueuedDownloader::processUrlQueue()
{
	std::lock_guard<std::mutex> locker(m_requestQueueMutex);

	foreach(const QUrl& url, m_requestQueue)
	{
		m_networkAccessManager->get(QNetworkRequest(url));
		m_requestQueue.erase(m_requestQueue.begin());
	}
}

void QueuedDownloader::stopProcessingUrlQueue()
{
	m_networkAccessManager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
}

void QueuedDownloader::startTimer(int interval, Qt::TimerType timerType)
{
	m_timerId = QObject::startTimer(interval, timerType);
}

void QueuedDownloader::killTimer(int timerId)
{
	QObject::killTimer(timerId);
}

}
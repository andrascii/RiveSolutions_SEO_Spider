#include "downloader.h"
#include "constants.h"

namespace QuickieWebBot
{

Downloader::Downloader()
	: QObject(nullptr)
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_timerId(0)
{
	moveToThread(&m_thisThread);

	VERIFY(connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), SLOT(urlDownloaded(QNetworkReply*))));
}

Downloader::~Downloader()
{
	stop();
	wait();
}

void Downloader::start()
{
	if (!m_thisThread.isRunning())
	{
		m_thisThread.start();

		QMetaObject::invokeMethod(this, "startTimer", Qt::BlockingQueuedConnection,
			Q_ARG(int, Common::g_minimumRecommendedTimerResolution), Q_ARG(Qt::TimerType, Qt::CoarseTimer));

		assert(m_timerId);
	}
}

void Downloader::stop()
{
	if (m_thisThread.isRunning())
	{
		assert(m_timerId);

		QMetaObject::invokeMethod(this, "killTimer",
			Qt::BlockingQueuedConnection, Q_ARG(int, m_timerId));

		m_thisThread.quit();
	}
}

void Downloader::wait()
{
	m_thisThread.wait();
}

void Downloader::scheduleRequest(const QUrl& url) noexcept
{
	QMutexLocker locker(&m_requestQueueMutex);
	m_requestQueue.push_back(url);
}

void Downloader::scheduleRequestList(const QList<QUrl>& urlList) noexcept
{
	QMutexLocker locker(&m_requestQueueMutex);
	m_requestQueue.append(urlList);
}

void Downloader::timerEvent(QTimerEvent* event)
{
	handleRequestQueue();
}

void Downloader::urlDownloaded(QNetworkReply* reply)
{
	QMutexLocker locker(&m_repliesQueueMutex);

	Response response{ reply->url(), reply->readAll() };

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		response.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
	}

	m_repliesQueue.push_back(std::move(response));
}

void Downloader::handleRequestQueue()
{
	QMutexLocker locker(&m_requestQueueMutex);

	foreach(const QUrl& url, m_requestQueue)
	{
		m_networkAccessManager->get(QNetworkRequest(url));
		m_requestQueue.erase(m_requestQueue.begin());
	}
}

void Downloader::stopHandlingRequestQueue()
{
	m_networkAccessManager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
}

void Downloader::startTimer(int interval, Qt::TimerType timerType)
{
	m_timerId = QObject::startTimer(interval, timerType);
}

void Downloader::killTimer(int timerId)
{
	QObject::killTimer(timerId);
}

}
#include "downloader.h"

namespace QuickieWebBot
{

Downloader::Downloader()
	: QObject(nullptr)
	, m_networkAccessManager(new QNetworkAccessManager(this))
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
	}
}

void Downloader::stop()
{
	if (!m_thisThread.isRunning())
	{
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

	QMetaObject::invokeMethod(this, "handleRequestQueue", Qt::QueuedConnection);
}

void Downloader::scheduleRequestList(const QList<QUrl>& urlList) noexcept
{
	QMutexLocker locker(&m_requestQueueMutex);
	m_requestQueue.append(urlList);

	QMetaObject::invokeMethod(this, "handleRequestQueue", Qt::QueuedConnection);
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
	}
}

void Downloader::stopHandlingRequestQueue()
{
	m_networkAccessManager->setNetworkAccessible(QNetworkAccessManager::NotAccessible);
}

}
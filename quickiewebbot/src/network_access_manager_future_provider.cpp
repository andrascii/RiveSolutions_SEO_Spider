#include "network_access_manager_future_provider.h"

namespace QuickieWebBot
{

QThread NetworkAccessManagerFutureProvider::s_networkAccessManagerThread;

NetworkAccessManagerFutureProvider::NetworkAccessManagerFutureProvider()
	: QObject(nullptr)
	, m_manager(new QNetworkAccessManager(this))
{
	VERIFY(connect(m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*))));

	qRegisterMetaType<NetworkSharedFuture>();

	moveToThread(&s_networkAccessManagerThread);
	s_networkAccessManagerThread.start();
}


NetworkAccessManagerFutureProvider::~NetworkAccessManagerFutureProvider()
{
	stopNetworAccessManagerThread();
}

NetworkSharedFuture NetworkAccessManagerFutureProvider::get(const QNetworkRequest& request) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	QNetworkReply* reply = nullptr;

	VERIFY(QMetaObject::invokeMethod(this, "getAtThreadContext", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG(QNetworkReply*, reply), Q_ARG(const QNetworkRequest&, request)));

	return m_responsePromiseMapper[reply].get_future();
}

NetworkSharedFuture NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, QIODevice* data) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	QNetworkReply* reply = nullptr;

	VERIFY(QMetaObject::invokeMethod(this, "postAtThreadContext", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG(QNetworkReply*, reply), Q_ARG(const QNetworkRequest&, request), Q_ARG(QIODevice*, data)));

	return m_responsePromiseMapper[reply].get_future();
}

NetworkSharedFuture NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, const QByteArray& data) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	QNetworkReply* reply = nullptr;

	VERIFY(QMetaObject::invokeMethod(this, "postAtThreadContext", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG(QNetworkReply*, reply), Q_ARG(const QNetworkRequest&, request), Q_ARG(const QByteArray&, data)));

	return m_responsePromiseMapper[reply].get_future();
}

NetworkSharedFuture NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept
{
	std::lock_guard<std::mutex> locker(m_mutex);

	QNetworkReply* reply = nullptr;

	VERIFY(QMetaObject::invokeMethod(this, "postAtThreadContext", Qt::BlockingQueuedConnection,
		Q_RETURN_ARG(QNetworkReply*, reply), Q_ARG(const QNetworkRequest&, request), Q_ARG(QHttpMultiPart*, multiPart)));

	return m_responsePromiseMapper[reply].get_future();
}

QNetworkReply* NetworkAccessManagerFutureProvider::getAtThreadContext(const QNetworkRequest& request) noexcept
{
	return m_manager->get(request);
}

QNetworkReply* NetworkAccessManagerFutureProvider::postAtThreadContext(const QNetworkRequest& request, QIODevice* data) noexcept
{
	return m_manager->post(request, data);
}

QNetworkReply* NetworkAccessManagerFutureProvider::postAtThreadContext(const QNetworkRequest& request, const QByteArray& data) noexcept
{
	return m_manager->post(request, data);
}

QNetworkReply* NetworkAccessManagerFutureProvider::postAtThreadContext(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept
{
	return m_manager->post(request, multiPart);
}

void NetworkAccessManagerFutureProvider::finished(QNetworkReply* reply)
{
	ResponsePack responsePack{ reply->readAll() };

	for (const QPair<QByteArray, QByteArray>& headerValuePair : reply->rawHeaderPairs())
	{
		responsePack.responseHeaderValuePairs += headerValuePair.first + ": " + headerValuePair.second + "\n";
	}

	m_responsePromiseMapper[reply].set_value(std::move(responsePack));
	m_responsePromiseMapper.erase(reply);
}

void NetworkAccessManagerFutureProvider::stopNetworAccessManagerThread()
{
	s_networkAccessManagerThread.quit();
}

}
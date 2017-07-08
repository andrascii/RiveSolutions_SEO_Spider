#include "network_access_manager_future_provider.h"

namespace QuickieWebBot
{

NetworkAccessManagerFutureProvider::NetworkAccessManagerFutureProvider(QNetworkAccessManager* manager, QObject* parent)
	: QObject(parent)
	, m_manager(manager)
{
	VERIFY(connect(m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(finished(QNetworkReply*))));
}

std::future<NetworkAccessManagerFutureProvider::ResponsePack>
NetworkAccessManagerFutureProvider::get(const QNetworkRequest& request) noexcept
{
	return m_responsePromiseMapper[m_manager->get(request)].get_future();
}

std::future<NetworkAccessManagerFutureProvider::ResponsePack>
NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, QIODevice* data) noexcept
{
	return m_responsePromiseMapper[m_manager->post(request, data)].get_future();
}

std::future<NetworkAccessManagerFutureProvider::ResponsePack>
NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, const QByteArray& data) noexcept
{
	return m_responsePromiseMapper[m_manager->post(request, data)].get_future();
}

std::future<NetworkAccessManagerFutureProvider::ResponsePack>
NetworkAccessManagerFutureProvider::post(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept
{
	return m_responsePromiseMapper[m_manager->post(request, multiPart)].get_future();
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

}
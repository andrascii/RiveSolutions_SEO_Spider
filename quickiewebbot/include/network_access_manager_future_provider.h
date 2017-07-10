#pragma once

namespace QuickieWebBot
{

class NetworkAccessManagerFutureProvider : public QObject
{
	Q_OBJECT

public:
	struct ResponsePack
	{
		QString responseBody;
		QString responseHeaderValuePairs;
	};

	NetworkAccessManagerFutureProvider();
	~NetworkAccessManagerFutureProvider();

	// thread-safe methods
	std::shared_future<ResponsePack> get(const QNetworkRequest& request) noexcept;
	std::shared_future<ResponsePack> post(const QNetworkRequest& request, QIODevice* data) noexcept;
	std::shared_future<ResponsePack> post(const QNetworkRequest& request, const QByteArray& data) noexcept;
	std::shared_future<ResponsePack> post(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept;

private:
	Q_SLOT void finished(QNetworkReply* reply);

	Q_SLOT QNetworkReply* getAtThreadContext(const QNetworkRequest& request) noexcept;
	Q_SLOT QNetworkReply* postAtThreadContext(const QNetworkRequest& request, QIODevice* data) noexcept;
	Q_SLOT QNetworkReply* postAtThreadContext(const QNetworkRequest& request, const QByteArray& data) noexcept;
	Q_SLOT QNetworkReply* postAtThreadContext(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept;

	static void stopNetworAccessManagerThread();

private:
	static QThread s_networkAccessManagerThread;

	std::mutex m_mutex;

	QNetworkAccessManager* m_manager;

	std::unordered_map<QNetworkReply*, std::promise<ResponsePack>> m_responsePromiseMapper;
};

using NetworkSharedFuture = std::shared_future<NetworkAccessManagerFutureProvider::ResponsePack>;

}

Q_DECLARE_METATYPE(QuickieWebBot::NetworkSharedFuture)
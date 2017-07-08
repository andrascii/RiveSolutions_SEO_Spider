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

	NetworkAccessManagerFutureProvider(QNetworkAccessManager* manager, QObject* parent = nullptr);

	std::future<ResponsePack> get(const QNetworkRequest& request) noexcept;
	std::future<ResponsePack> post(const QNetworkRequest& request, QIODevice* data) noexcept;
	std::future<ResponsePack> post(const QNetworkRequest& request, const QByteArray& data) noexcept;
	std::future<ResponsePack> post(const QNetworkRequest& request, QHttpMultiPart* multiPart) noexcept;

protected:
	Q_SLOT void finished(QNetworkReply* reply);

private:
	QNetworkAccessManager* m_manager;
	mutable std::mutex m_mutex;
	std::map<QNetworkReply*, std::promise<ResponsePack>> m_responsePromiseMapper;
};

}

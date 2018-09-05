#include "yandex_metrica_data_feed.h"
#include "custom_uri_channel.h"
#include "crawler_worker_thread.h"
#include "download_request.h"
#include "download_response.h"

namespace CrawlerEngine
{
YandexMetricaAuthenticator::YandexMetricaAuthenticator(QObject* parent)
	: QObject(parent)
{
	VERIFY(connect(CustomUrlChannel::instance(), &CustomUrlChannel::uriReceived, 
		this, &YandexMetricaAuthenticator::onCustomUriReceived, Qt::QueuedConnection));
}

bool YandexMetricaAuthenticator::authenticated() const
{
	return m_token.length() > 0 && m_tokenExpireTime.isValid() && m_tokenExpireTime > QDateTime::currentDateTime();
}

void YandexMetricaAuthenticator::authenticate()
{
	QDesktopServices::openUrl(QUrl("https://oauth.yandex.ru/authorize?response_type=token&client_id=61da2c3a9eed4b5b938c1aba793d1b18"));
}

QByteArray YandexMetricaAuthenticator::token() const
{
	return m_token;
}

void YandexMetricaAuthenticator::onCustomUriReceived(const QByteArray& url)
{
	// seospider://yandexmetrica/#access_token=AQAAAAAGVrMJAAUs-4OXrgBu90CPvT_AgaSN-HE&token_type=bearer&expires_in=31536000 
	if (url.startsWith("seospider://yandexmetrica"))
	{
		if (url.contains("access_token=") && url.contains("expires_in"))
		{
			const int accessTokenStart = url.indexOf("access_token=") + 13;
			const int accessTokenEnd = url.indexOf("&", accessTokenStart);

			const int expiresStart = url.indexOf("expires_in=") + 11;
			const int expiresend = url.indexOf("&", expiresStart);

			m_token = url.mid(accessTokenStart, accessTokenEnd != -1 ? accessTokenEnd - accessTokenStart: -1);
			const QByteArray expires = url.mid(expiresStart, expiresend != -1 ? expiresend - expiresStart : -1);
			m_tokenExpireTime = QDateTime::currentDateTime().addSecs(expires.toInt());

			QVariantMap result;
			result["token"] = m_token;
			result["expires"] = m_tokenExpireTime;

			INFOLOG << m_token << m_tokenExpireTime.toString();

			emit authenticationDone(result);
		}
	}
}

YandexMetricaDataFeed::YandexMetricaDataFeed()
	: m_authenticator(new YandexMetricaAuthenticator(this))
{
	// just for test
	m_authenticator->authenticate();
}

DataFeedId YandexMetricaDataFeed::dataFeedId() const
{
	return DataFeedId::YandexMetrica;
}

QString YandexMetricaDataFeed::name() const
{
	return QObject::tr("Yandex Metrika");
}

QStringList YandexMetricaDataFeed::columns() const
{
	// todo: implement
	return QStringList() << "Yandex Metrica Column 1";
}

void YandexMetricaDataFeed::requestData(ParsedPageWeakPtr page)
{
	// todo: implement
	Q_UNUSED(page);

	if (auto lockedPage = page.lock())
	{
		// TODO: generate correct url
		const Url apiUrl = QString("https://api-metrika.yandex.ru/stat/v1/data?dimensions=ym:pv:URLPath=~") + lockedPage->url.path(QUrl::FullyEncoded)
			+ "&oauth_token=" + m_authenticator->token();
		
		DownloadRequest request({apiUrl , DownloadRequestType::RequestTypeGet });
		m_downloadRequester.reset(request, this, &YandexMetricaDataFeed::onLoadingDone);
		
		m_downloadRequester->start();
	}
}

bool YandexMetricaDataFeed::connected() const
{
	return m_authenticator->authenticated();
}

void YandexMetricaDataFeed::connect()
{
	m_authenticator->authenticate();
}

void YandexMetricaDataFeed::setConnectionData(const QVariantMap& data)
{
	// TODO: implement
	// choose account, etc
	Q_UNUSED(data);
}

ICustomDataFeedAuthenticator* YandexMetricaDataFeed::authenticator() const
{
	return m_authenticator;
}

QVariantMap YandexMetricaDataFeed::connectionData() const
{
	// TODO: implement
	return QVariantMap();
}

void YandexMetricaDataFeed::setParams(const QVariantMap& params)
{
	// TODO: implement
	Q_UNUSED(params);
}

void YandexMetricaDataFeed::onLoadingDone(Requester* requester, const DownloadResponse& response)
{
	Q_UNUSED(requester);
	Q_UNUSED(response);
}
}

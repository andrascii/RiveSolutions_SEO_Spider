#include "http_client.h"
#include "curl_http_get_request.h"
#include "service_locator.h"
#include "inotification_service.h"
#include "curl_http_post_request.h"
#include "random_interval_range_timer.h"

namespace
{

using namespace CrawlerEngine;

CURL* initCurl()
{
	CURL* const result = curl_easy_init();

	if (!result)
	{
		const QString errorMessage = QObject::tr("Something was wrong. We cannot initialize network library to get job done.");

		ServiceLocator* serviceLocator = ServiceLocator::instance();

		serviceLocator->service<INotificationService>()->info(
			QObject::tr("Network Error"), errorMessage);

		ERRLOG << errorMessage;
	}

	return result;
}

}

namespace CrawlerEngine
{

HttpClient::HttpClient(QObject* parent)
	: QObject(parent)
	, m_connection(initCurl())
	, m_randomIntervalRangeTimer(new RandomIntervalRangeTimer(this))
	, m_timeout(-1)
	, m_maxRedirectCount(-1)
{
	VERIFY(connect(m_randomIntervalRangeTimer, &RandomIntervalRangeTimer::timerTicked,
		this, &HttpClient::onTimerTicked, Qt::DirectConnection));
}

HttpClient::~HttpClient()
{
	if (m_connection)
	{
		curl_easy_cleanup(m_connection);
	}
}

void HttpClient::get(const Url& url)
{
	if (m_randomIntervalRangeTimer->isValid())
	{
		m_pendingRequestsQueue.push_back(PendingNetworkRequest{ url, Method::Get });
		return;
	}

	getInternal(url);
}

void HttpClient::post(const Url& url, const QByteArray& uploadData)
{
	if (m_randomIntervalRangeTimer->isValid())
	{
		m_pendingRequestsQueue.push_back(PendingNetworkRequest{ url, Method::Post, uploadData });
		return;
	}

	postInternal(url, uploadData);
}

void HttpClient::setPauseRange(int from, int to)
{
	resetPauseRange();
	m_randomIntervalRangeTimer->setRange(from, to);
	m_randomIntervalRangeTimer->start();
}

void HttpClient::resetPauseRange()
{
	m_randomIntervalRangeTimer->reset();
}

void HttpClient::setTimeout(int msecs)
{
	m_timeout = msecs;
}

void HttpClient::setMaxRedirects(int redirects)
{
	m_maxRedirectCount = redirects;
}

void HttpClient::setUserAgent(const QByteArray& userAgent)
{
	m_userAgent = userAgent;
}

void HttpClient::setProxy(const QString& proxyHostName, int proxyPort, const QString& proxyUser, const QString& proxyPassword)
{
	m_proxySettings.proxyHostName = proxyHostName;
	m_proxySettings.proxyPort = proxyPort;
	m_proxySettings.proxyUser = proxyUser;
	m_proxySettings.proxyPassword = proxyPassword;
}

void HttpClient::resetProxy()
{
	m_proxySettings = ProxySettings();
}

QObject* HttpClient::qobject() const
{
	return const_cast<QObject*>(qobject_cast<const QObject*>(this));
}

void HttpClient::applyRequestSettings(CurlHttpRequest& request) const
{
	if (m_timeout != -1)
	{
		request.setTimeout(m_timeout);
	}

	if (m_maxRedirectCount != -1)
	{
		request.setMaxRedirects(m_maxRedirectCount);
	}

	if (!m_userAgent.isEmpty())
	{
		request.addRequestHeader("User-Agent", m_userAgent);
	}

	if (!m_proxySettings.proxyHostName.isEmpty())
	{
		request.setProxy(m_proxySettings.proxyHostName,
			m_proxySettings.proxyPort,
			m_proxySettings.proxyUser,
			m_proxySettings.proxyPassword);
	}
}

void HttpClient::getInternal(const Url& url)
{
	CurlHttpGetRequest request(m_connection, url);
	applyRequestSettings(request);

	const HopsChain hopsChain = request.execute();
	operationCompleted(hopsChain);
}

void HttpClient::postInternal(const Url& url, const QByteArray& uploadData)
{
	CurlHttpPostRequest request(m_connection, url, uploadData);
	applyRequestSettings(request);

	const HopsChain hopsChain = request.execute();
	operationCompleted(hopsChain);
}

void HttpClient::onTimerTicked()
{
	DEBUG_ASSERT(thread() == QThread::currentThread());

	if (m_pendingRequestsQueue.empty())
	{
		return;
	}

	const PendingNetworkRequest pendingNetworkRequest(std::move(m_pendingRequestsQueue.front()));

	m_pendingRequestsQueue.pop_front();

	switch(pendingNetworkRequest.method)
	{
		case Method::Get:
		{
			return getInternal(pendingNetworkRequest.url);
		}
		case Method::Post:
		{
			return postInternal(pendingNetworkRequest.url, pendingNetworkRequest.uploadData);
		}
		default:
		{
			ASSERT(!"Unknown method");
		}
	}
}

}

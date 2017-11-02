
#include "robots_txt_loader.h"
#include "status_code.h"

namespace CrawlerEngine
{

RobotsTxtLoader::RobotsTxtLoader(QNetworkAccessManager* networkAccessor)
	: m_isReady(false)
	, m_networkAccessor(networkAccessor)
{
}

void RobotsTxtLoader::setUserAgent(const QByteArray& userAgent)
{
	m_userAgent = userAgent;
}

void RobotsTxtLoader::load(const QUrl& url)
{
	ASSERT(m_networkAccessor->thread() == QThread::currentThread());

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*))));

	const QString robotsTxtUrlString = url.scheme() + "://" + url.host() + QStringLiteral("/robots.txt");

	QNetworkRequest request(robotsTxtUrlString);
	request.setRawHeader("User-Agent", m_userAgent);

	QNetworkReply* reply = m_networkAccessor->get(request);
}

const QByteArray& RobotsTxtLoader::content() const noexcept
{
	return m_content;
}

bool RobotsTxtLoader::isReady() const noexcept
{
	return m_isReady;
}

QObject* RobotsTxtLoader::qobject()
{
	return this;
}

void RobotsTxtLoader::onLoadingDone(QNetworkReply* reply)
{
	const QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

	if (!redirectUrl.isEmpty())
	{
		QNetworkRequest request(redirectUrl);
		request.setRawHeader("User-Agent", m_userAgent);

		m_networkAccessor->get(request);
		return;
	}

	disconnect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*)));

	const QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	const Common::StatusCode statusCodeValue = statusCode.isValid() ?
		static_cast<Common::StatusCode>(statusCode.toInt()) :
		Common::StatusCode::Undefined;

	if (statusCodeValue == Common::StatusCode::Undefined)
	{
		return;
	}

	m_content = reply->readAll();
	m_isReady = true;

	emit ready();
}

}

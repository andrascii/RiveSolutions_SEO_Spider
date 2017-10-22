
#include "robots_txt_loader.h"
#include "status_code.h"

namespace WebCrawler
{

RobotsTxtLoader::RobotsTxtLoader(QNetworkAccessManager* networkAccessor)
	: m_isReady(false)
	, m_networkAccessor(networkAccessor)
{
}

void RobotsTxtLoader::load(const QUrl& url)
{
	ASSERT(m_networkAccessor->thread() == QThread::currentThread());

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*))));

	const QString robotsTxt = url.scheme() + "://" + url.host() + QStringLiteral("/robots.txt");
	QNetworkReply* reply = m_networkAccessor->get(QNetworkRequest(robotsTxt));
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
		m_networkAccessor->get(QNetworkRequest(redirectUrl));
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

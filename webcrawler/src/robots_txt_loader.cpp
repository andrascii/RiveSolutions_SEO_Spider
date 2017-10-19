
#include "robots_txt_loader.h"
#include "status_code.h"

namespace WebCrawler
{

namespace
{

	class AnywayFunctionCall
	{
	public:
		AnywayFunctionCall(const std::function<void()>& function)
			: m_function(function)
		{
		}

		~AnywayFunctionCall()
		{
			m_function();
		}

	private:
		std::function<void()> m_function;
	};

}

RobotsTxtLoader::RobotsTxtLoader(QNetworkAccessManager* networkAccessor)
	: m_networkAccessor(networkAccessor)
{

}

void RobotsTxtLoader::load(const QUrl& url)
{
	ASSERT(m_networkAccessor->thread() == QThread::currentThread());

	VERIFY(connect(m_networkAccessor, SIGNAL(finished(QNetworkReply*)), this, SLOT(onLoadingDone(QNetworkReply*))));

	const QString robotsTxt = url.scheme() + "://" + url.host() + QStringLiteral("/robots.txt");
	QNetworkReply* reply = m_networkAccessor->get(QNetworkRequest(robotsTxt));
}

QObject* RobotsTxtLoader::qObject()
{
	return this;
}

void RobotsTxtLoader::onLoadingDone(QNetworkReply* reply)
{
	//AnywayFunctionCall anywayFunctionCall(std::function<void()>(std::bind(&RobotsTxtRules::initialized, this)));

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

	emit ready(reply->readAll());
}

}

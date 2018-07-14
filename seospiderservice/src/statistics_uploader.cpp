#include "statistics_uploader.h"
#include "seo_spider_service_app.h"
#include "helpers.h"

namespace SeoSpiderService
{

StatisticsUploader::StatisticsUploader(QObject* parent)
	: QObject(parent)
	, m_eventLoop(new QEventLoop(this))
	, m_networkAccessManager(new QNetworkAccessManager(this))
	, m_isRunning(false)
{
	connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &StatisticsUploader::onUploadFinished);
	connect(m_networkAccessManager, &QNetworkAccessManager::finished, m_eventLoop, &QEventLoop::quit);
	connect(m_networkAccessManager, &QNetworkAccessManager::authenticationRequired, this, &StatisticsUploader::onAuthenticationRequired);
}

void StatisticsUploader::configurateStatisticsFile()
{
	SeoSpiderServiceApp* app = qobject_cast<SeoSpiderServiceApp*>(qApp);

	m_statisticsFile.setFileName(app->statisticsFilePath());

	static QFileInfo statisticsFileInfo(m_statisticsFile);

	m_statisticsFileName = statisticsFileInfo.fileName();
}

void StatisticsUploader::configurateManager()
{
	m_requestUrl = s_statisticsServerUrl + m_statisticsFileName;

	m_requestUrl.setUserName(s_statisticsServerUser);
	m_requestUrl.setPassword(s_statisticsServerPassword);
	m_requestUrl.setPort(s_statisticsServerPort);
}

void StatisticsUploader::onUploadFinished(QNetworkReply* reply)
{
	if (!reply->error())
	{
		m_statisticsFile.close();
		reply->deleteLater();
	}

	emit uploadFinished();
}

void StatisticsUploader::onAuthenticationRequired(QNetworkReply*, QAuthenticator* authenticator)
{
	authenticator->setUser(s_statisticsServerUser);
	authenticator->setPassword(s_statisticsServerPassword);
}

void StatisticsUploader::startUploading()
{
	if(m_isRunning)
	{
		return;
	}

	m_isRunning = true;

	configurateStatisticsFile();

	configurateManager();

	if (m_statisticsFile.open(QIODevice::ReadOnly) && !m_eventLoop->isRunning())
	{
		m_networkAccessManager->put(QNetworkRequest(m_requestUrl), &m_statisticsFile);

		m_eventLoop->exec();
	}
}

}

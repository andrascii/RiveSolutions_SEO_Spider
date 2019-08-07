#pragma once

namespace
{
//
//TODO: use ftps
//
const QString s_statisticsServerUrl = "ftp://rivesolutions.com/";
const QString s_statisticsServerUser = "j775420_statistics";
const QString s_statisticsServerPassword = "8462";
const uint s_statisticsServerPort = 21; /* ftp port */

}

namespace SeoSpiderService
{

class StatisticsUploader : public QObject
{
	Q_OBJECT

public:
	StatisticsUploader(QObject* parent = nullptr);

public slots:
	void startUploading();

signals:
	void errorOccured(QString errorDescription);
	void uploadFinished();

private:
	void configurateStatisticsFile();
	void configurateManager();

private slots:
	void onUploadFinished(QNetworkReply *reply);
	void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
	void onReplyError(QNetworkReply::NetworkError code);

private:
	QEventLoop* m_eventLoop;

	QNetworkReply* m_reply;
	QNetworkAccessManager* m_networkAccessManager;
	QUrl m_requestUrl;

	QFile m_statisticsFile;
	QString m_statisticsFileName;

	bool m_isRunning;
};

}
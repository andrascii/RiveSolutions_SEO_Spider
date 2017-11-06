#include "tests_downloader.h"
#include "download_response.h"
#include "download_request.h"
#include "thread_queue.h"
#include "handler_registry.h"

namespace CrawlerEngineTests
{

TestsDownloader::TestsDownloader()
{
	Common::HandlerRegistry& handlerRegistry = Common::HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, Common::RequestType::RequestTypeDownload);
}

TestsDownloader::~TestsDownloader()
{

}

void TestsDownloader::setUserAgent(const QByteArray& userAgent)
{
}

void TestsDownloader::handleRequest(Common::RequesterSharedPtr requester)
{
	std::shared_ptr<CrawlerEngine::DownloadResponse> response = 
		std::make_shared<CrawlerEngine::DownloadResponse>();

	CrawlerEngine::DownloadRequest* request = static_cast<CrawlerEngine::DownloadRequest*>(requester->request());

	const QUrl requestedUrl = request->requestInfo.url;

	response->statusCode = 200;
	response->url = requestedUrl;

	std::pair<QString, QString> files = mapUrlToTestDataFiles(request->requestInfo);

	if (!files.first.isEmpty())
	{
		QFile inputFile(files.second);

		ASSERT(inputFile.open(QIODevice::ReadOnly));

		QTextStream in(&inputFile);

		while (!in.atEnd())
		{
			const QString line = in.readLine();
			const QStringList headerAndValue = line.split(":");

			const QByteArray header = headerAndValue.first().trimmed().toLatin1();
			const QByteArray value = headerAndValue.last().trimmed().toLatin1();

			if (header.toLower() == "status-code")
			{
				bool ok;
				response->statusCode = value.toInt(&ok);

				ASSERT(ok);
			}
			else
			{
				response->responseHeaders.push_back(QPair<QByteArray, QByteArray>(header, value));
			}
		}

		inputFile.close();

		if (request->requestInfo.requestType == CrawlerEngine::RequestTypeGet)
		{
			QFile html(files.first);

			ASSERT(html.open(QIODevice::ReadOnly));

			response->responseBody = html.readAll();

			html.close();
		}
	}
	else
	{
		response->statusCode = 404;
	}

	Common::ThreadQueue::forThread(requester->thread())->postResponse(requester, response);
}

void TestsDownloader::stopRequestHandling(Common::RequesterSharedPtr requester)
{
	requester;
}

QObject* TestsDownloader::qobject()
{
	return this;
}

QDir TestsDownloader::testsDataDir() const
{
	if (m_testDataPath.isEmpty())
	{
		QDir dir = QFileInfo(__FILE__).absoluteDir();

		while (m_testDataPath.isEmpty())
		{
			const QString testsDataPath1 = QDir::cleanPath(dir.absolutePath() + "/crawlerenginetests/testsdata");
			const QDir testsDataDir1 = QDir(testsDataPath1);

			const QString testsDataPath2 = QDir::cleanPath(dir.absolutePath() + "/testsdata");
			const QDir testsDataDir2 = QDir(testsDataPath2);

			if (testsDataDir1.exists())
			{
				m_testDataPath = testsDataPath1;
				break;
			}

			if (testsDataDir2.exists())
			{
				m_testDataPath = testsDataPath2;
				break;
			}

			if (!dir.cdUp())
			{
				break;
			}
		}

		ASSERT(!m_testDataPath.isEmpty());
	}

	return QDir(m_testDataPath);
}

std::pair<QString, QString> TestsDownloader::mapUrlToTestDataFiles(const CrawlerEngine::CrawlerRequest& url) const
{
	const QDir testsDir = testsDataDir();
	const QString hostPath = url.url.host();

	QString path = url.url.path(QUrl::FullyEncoded);

	if (path.isEmpty())
	{
		path = "index.html";
	}

	QString requestedFilePath = QDir::cleanPath(testsDir.absolutePath() + "/" + hostPath + "/" + path);
	QString metadataFilePath = requestedFilePath + ".meta";

	const bool metaFileExists = QFileInfo::exists(metadataFilePath);
	const bool requestedFileExists = QFileInfo::exists(requestedFilePath);

	ASSERT(metaFileExists && requestedFileExists || !metaFileExists && !requestedFileExists);

	if (!metaFileExists || !requestedFileExists)
	{
		return std::make_pair(QString::null, QString::null);
	}

	return std::make_pair(requestedFilePath, metadataFilePath);
}

}
#include "tests_queued_downloader.h"

namespace CrawlerEngineTests
{

void TestsQueudedDownoader::setUserAgent(const QByteArray& userAgent)
{
	// do nothing
}

void TestsQueudedDownoader::scheduleUrl(const CrawlerEngine::CrawlerRequest& url) noexcept
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);
	Reply reply;
	reply.statusCode = 200;
	reply.url = url.url;

	std::pair<QString, QString> files = mapUrlToTestDataFiles(url);

	if (!files.first.isEmpty())
	{
		QFile inputFile(files.second);
		ASSERT(inputFile.open(QIODevice::ReadOnly));
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			QStringList headerAndValue = line.split(":");
			QByteArray header = headerAndValue.first().trimmed().toLatin1();
			QByteArray value = headerAndValue.last().trimmed().toLatin1();

			if (header.toLower() == "status-code")
			{
				bool ok;
				reply.statusCode = value.toInt(&ok);
				ASSERT(ok);
			}
			else
			{
				reply.responseHeaders.push_back(QPair<QByteArray, QByteArray>(header, value));
			}
			
		}
		inputFile.close();

		if (url.requestType == CrawlerEngine::RequestTypeGet)
		{
			QFile html(files.first);
			ASSERT(html.open(QIODevice::ReadOnly));
			reply.responseBody = html.readAll();
			html.close();
		}
	}
	else
	{
		reply.statusCode = 404;
	}

	m_repliesQueue.push_back(reply);
}

bool TestsQueudedDownoader::extractReply(Reply& response) noexcept
{
	std::lock_guard<std::mutex> locker(m_repliesQueueMutex);

	if (m_repliesQueue.empty())
	{
		return false;
	}

	response = std::move(*m_repliesQueue.begin());
	m_repliesQueue.erase(m_repliesQueue.begin());

	return true;
}

void TestsQueudedDownoader::start() noexcept
{
	// do nothing
}

void TestsQueudedDownoader::stop() noexcept
{
	// do nothing
}


size_t TestsQueudedDownoader::unprocessedRequestCount() const noexcept
{
	return 0;
}

QDir TestsQueudedDownoader::testsDataDir() const
{
	if (m_testDataPath.isEmpty())
	{
		//QDir dir = QDir::current(); // working directory is inside qt bins
		QDir dir = QFileInfo(__FILE__).absoluteDir();
		while (m_testDataPath.isEmpty())
		{
			QString testsDataPath1 = QDir::cleanPath(dir.absolutePath() + "/crawlerenginetests/testsdata");
			QDir testsDataDir1 = QDir(testsDataPath1);

			QString testsDataPath2 = QDir::cleanPath(dir.absolutePath() + "/testsdata");
			QDir testsDataDir2 = QDir(testsDataPath2);

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

std::pair<QString, QString> TestsQueudedDownoader::mapUrlToTestDataFiles(const CrawlerEngine::CrawlerRequest& url) const
{
	QDir testsDir = testsDataDir();
	QString hostPath = url.url.host();
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

#include "tests_downloader.h"
#include "download_response.h"
#include "download_request.h"
#include "thread_message_dispatcher.h"
#include "handler_registry.h"

namespace CrawlerEngineTests
{

using namespace CrawlerEngine;

TestsDownloader::TestsDownloader()
{
	CrawlerEngine::HandlerRegistry& handlerRegistry = CrawlerEngine::HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, CrawlerEngine::RequestType::RequestTypeDownload);

	m_responsePostProcessor = [](CrawlerEngine::DownloadResponse&) {};
}

TestsDownloader::~TestsDownloader()
{
	CrawlerEngine::HandlerRegistry& handlerRegistry = CrawlerEngine::HandlerRegistry::instance();
	handlerRegistry.unregistrateHandler(this);
}

void TestsDownloader::setPauseRange(int, int)
{
}

void TestsDownloader::resetPauseRange()
{
}

void TestsDownloader::setUserAgent(const QByteArray&)
{
}

void TestsDownloader::setProxy(const QString&, int, const QString&, const QString&)
{
}

void TestsDownloader::resetProxy()
{
}

void TestsDownloader::handleRequest(CrawlerEngine::RequesterSharedPtr requester)
{
	std::shared_ptr<CrawlerEngine::DownloadResponse> response = 
		std::make_shared<CrawlerEngine::DownloadResponse>();

	CrawlerEngine::DownloadRequest* request = static_cast<CrawlerEngine::DownloadRequest*>(requester->request());

	QSet<QString> uniqueUrls;
	response->hopsChain = hopsChain(*request, uniqueUrls);
	m_responsePostProcessor(*(response.get()));

	CrawlerEngine::ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);
}

void TestsDownloader::stopRequestHandling(CrawlerEngine::RequesterSharedPtr requester)
{
	requester;
}

QObject* TestsDownloader::qobject()
{
	return this;
}

void TestsDownloader::setPostProcessor(PostProcessorFunc postProcessor)
{
	m_responsePostProcessor = postProcessor;
}

HopsChain TestsDownloader::hopsChain(const DownloadRequest& request, QSet<QString>& uniqueUrls) const
{
	HopsChain result;

	const Url requestedUrl = request.requestInfo.url;

	Hop hop;
	hop.setStatusCode(Common::StatusCode::Ok200);
	hop.setUrl(requestedUrl);

	QString redirectUrl;

	std::pair<QString, QString> files = mapUrlToTestDataFiles(request);

	if (!files.first.isEmpty())
	{
		QFile inputFile(files.second);

		ASSERT(inputFile.open(QIODevice::ReadOnly));

		QTextStream in(&inputFile);

		ResponseHeaders responseHeaders;

		while (!in.atEnd())
		{
			const QString line = in.readLine();
			const int separatorIndex = line.indexOf(":");
			if (separatorIndex == -1)
			{
				continue;
			}

			const QByteArray header = line.mid(0, separatorIndex).trimmed().toLatin1();
			const QByteArray value = line.mid(separatorIndex + 1).trimmed().toLatin1();

			if (header.toLower() == "status-code")
			{
				bool ok;
				hop.setStatusCode(static_cast<Common::StatusCode>(value.toInt(&ok)));

				ASSERT(ok);
			}
			else if (header.toLower() == "redirect-url")
			{
				redirectUrl = QString::fromUtf8(value);

				responseHeaders.addHeaderValue("Location", redirectUrl);
				hop.setRedirectUrl(redirectUrl);
			}
			else
			{
				responseHeaders.addHeaderValue(header, value);
			}
		}

		hop.setResponseHeaders(responseHeaders);

		inputFile.close();

		if (request.requestInfo.requestType == CrawlerEngine::DownloadRequestType::RequestTypeGet)
		{
			QFile html(files.first);

			ASSERT(html.open(QIODevice::ReadOnly));

			hop.setBody(html.readAll());

			html.close();
		}
	}
	else
	{
		hop.setStatusCode(Common::StatusCode::NotFound404);
	}

	result.addHop(hop);
	uniqueUrls.insert(request.requestInfo.url.urlStr());

	if (!redirectUrl.isEmpty() && !uniqueUrls.contains(redirectUrl))
	{
		DownloadRequest childRequest = request;
		childRequest.requestInfo.url = Url(PageParserHelpers::resolveRelativeUrl(redirectUrl, requestedUrl));
		HopsChain childResult = hopsChain(childRequest, uniqueUrls);

		for (size_t i = 0; i < childResult.length(); ++i)
		{
			result.addHop(childResult[i]);
		}

		if (childResult.length() > 0)
		{
			result.front().setRedirectUrl(result[1].url());
		}
	}

	return result;
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

std::pair<QString, QString> TestsDownloader::mapUrlToTestDataFiles(const CrawlerEngine::DownloadRequest& downloadRequest) const
{
	const QDir testsDir = testsDataDir();
	const QString hostPath = downloadRequest.requestInfo.url.host();

	QString path = downloadRequest.requestInfo.url.path(Url::FullyEncoded);

	if (path.isEmpty())
	{
		path = "index.html";
	}

	QString requestedFilePath = QDir::cleanPath(testsDir.absolutePath() + "/" + hostPath + "/" + path);

	if (QFileInfo(requestedFilePath).isDir())
	{
		requestedFilePath = requestedFilePath.endsWith(QChar('/')) ? 
			requestedFilePath + QString("index.html") : 
			requestedFilePath + QString("/index.html");
	}

	QString filename = downloadRequest.requestInfo.url.fileName();

	if (downloadRequest.linkStatus == DownloadRequest::LinkStatus::LinkStatusReloadAlreadyLoaded)
	{
		const int lastDotIndex = filename.lastIndexOf(".");
		filename.insert(lastDotIndex, "_fixed");

		const int lastPathSlashIndex = requestedFilePath.lastIndexOf("/");
		requestedFilePath.remove(lastPathSlashIndex + 1, requestedFilePath.size() - lastPathSlashIndex + 1);
		requestedFilePath.append(filename);
	}

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
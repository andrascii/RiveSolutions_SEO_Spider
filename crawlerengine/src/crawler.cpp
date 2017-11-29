#include "crawler.h"
#include "unordered_data_collection.h"
#include "sequenced_data_collection.h"
#include "model_controller.h"
#include "crawler_worker_thread.h"
#include "robots_txt_rules.h"
#include "robots_txt_loader.h"
#include "thread_manager.h"
#include "downloader.h"
#include "host_info_provider.h"
#include "site_map.h"
#include "json_parser_stream_writer.h"
#include "serializer.h"

namespace CrawlerEngine
{

Crawler* Crawler::s_instance = nullptr;

Crawler& Crawler::instance()
{
	return *s_instance;
}

Crawler::Crawler(unsigned int threadCount, QObject* parent)
	: QObject(parent)
	, m_robotsTxtLoader(nullptr)
	, m_modelController(nullptr)
	, m_uniqueLinkStore(new UniqueLinkStore(this))
	, m_theradCount(threadCount)
	, m_crawlingStateTimer(new QTimer(this))
	, m_sequencedDataCollection(nullptr)
	, m_state(StatePending)
{
	ASSERT(s_instance == nullptr && "Allowed only one instance of Crawler");

	ASSERT(qRegisterMetaType<ParsedPagePtr>());
	ASSERT(qRegisterMetaType<CrawlingProgress>());
	ASSERT(qRegisterMetaType<CrawlerOptions>() > -1);
	ASSERT(qRegisterMetaType<RobotsTxtRules>());

	VERIFY(connect(m_crawlingStateTimer, &QTimer::timeout, this, &Crawler::onAboutCrawlingState));
	
	m_crawlingStateTimer->setInterval(100);

	s_instance = this;
}

Crawler::~Crawler()
{
	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	ThreadManager::destroy();

	s_instance = nullptr;
}

void Crawler::initialize()
{
	m_modelController = new ModelController;

	ThreadManager::instance().moveObjectToThread(createDownloader()->qobject(), "DownloaderThread");
	ThreadManager::instance().moveObjectToThread(new HostInfoProvider, "BackgroundThread");
	ThreadManager::instance().moveObjectToThread(m_modelController, "BackgroundThread");

	for (unsigned i = 0; i < m_theradCount; ++i)
	{
		m_workers.push_back(new CrawlerWorkerThread(m_uniqueLinkStore));

		VERIFY(connect(m_workers.back(), SIGNAL(pageParsed(ParsedPagePtr)),
			m_modelController, SLOT(addParsedPage(ParsedPagePtr)), Qt::QueuedConnection));

		ThreadManager::instance().moveObjectToThread(m_workers.back(),
			QString("CrawlerWorkerThread#%1").arg(i).toLatin1());
	}
}

void Crawler::clearData()
{
	ASSERT(m_state == StatePending || m_state == StatePause);

	VERIFY(QMetaObject::invokeMethod(m_modelController, "clearData", Qt::BlockingQueuedConnection));

	m_uniqueLinkStore->clear();

	m_state = StatePending;

	emit stateChanged(m_state);
	emit onAboutClearData();
}

bool Crawler::isNoData() const noexcept
{
	return m_sequencedDataCollection->empty();
}

CrawlerEngine::Crawler::State Crawler::state() const noexcept
{
	return m_state;
}

void Crawler::startCrawling(const CrawlerOptions& options)
{
	m_options = options;

	m_state = StateWorking;

	initializeCrawlingSession();

	emit stateChanged(m_state);
}

void Crawler::stopCrawling()
{
	if (m_state == StatePause || m_state == StatePending)
	{
		return;
	}

	m_state = StatePause;

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "stop", Qt::BlockingQueuedConnection));
	}

	m_crawlingStateTimer->stop();

	emit crawlerStopped();
	emit stateChanged(m_state);

	INFOLOG << "crawler stopped";
}

void Crawler::onAboutCrawlingState()
{
	CrawlingProgress progress;

	const size_t seqCollCount = m_sequencedDataCollection->crawledStorageSize();
	const size_t controllerCrawled = m_modelController->crawledStorageSize();
	const size_t controllerAccepted = m_modelController->acceptedCrawledStorageSize();

	const size_t linkStoreCrawled = uniqueLinkStore()->crawledLinksCount();
	const size_t linkStorePending = uniqueLinkStore()->pendingLinksCount();

	const size_t controllerPending = qMax(linkStoreCrawled, controllerCrawled) - controllerCrawled;
	const size_t seqCollPending = qMax(controllerAccepted, seqCollCount) - seqCollCount;
	const size_t additionalPendingCount = controllerPending + seqCollPending;

	progress.crawledLinkCount = linkStoreCrawled - additionalPendingCount;
	progress.pendingLinkCount = linkStorePending + additionalPendingCount;

	emit crawlingProgress(progress);
}

void Crawler::onCrawlingSessionInitialized()
{
	if (!isPreinitialized())
	{
		return;
	}

	VERIFY(QMetaObject::invokeMethod(m_modelController, "setWebCrawlerOptions", 
		Qt::BlockingQueuedConnection, Q_ARG(const CrawlerOptions&, m_options)));

	m_uniqueLinkStore->addUrl(m_options.host, DownloadRequestType::RequestTypeGet);

	for (CrawlerWorkerThread* worker : m_workers)
	{
		VERIFY(QMetaObject::invokeMethod(worker, "startWithOptions", Qt::QueuedConnection, 
			Q_ARG(const CrawlerOptions&, m_options), Q_ARG(RobotsTxtRules, RobotsTxtRules(robotsTxtLoader()->content()))));
	}

	m_crawlingStateTimer->start();

	emit crawlerStarted();

	INFOLOG << "crawler started";
}

bool Crawler::isPreinitialized() const
{
	return robotsTxtLoader()->isReady();
}

void Crawler::initializeCrawlingSession()
{
	DEBUG_ASSERT(m_options.host.isValid());

	VERIFY(connect(robotsTxtLoader()->qobject(), SIGNAL(ready()), 
		this, SLOT(onCrawlingSessionInitialized()), Qt::QueuedConnection));

	robotsTxtLoader()->load(m_options.host);
}

void Crawler::createSequencedDataCollection(QThread* targetThread) const
{
	m_sequencedDataCollection.reset(m_modelController->data()->createSequencedDataCollection(targetThread));
}

IRobotsTxtLoader* Crawler::robotsTxtLoader() const
{
	if (!m_robotsTxtLoader)
	{
		m_robotsTxtLoader.reset(createRobotsTxtLoader());

		m_robotsTxtLoader->setUserAgent(m_options.plainUserAgent);
	}

	return m_robotsTxtLoader.get();
}

IDownloader* Crawler::createDownloader() const
{
	IDownloader* downloader = new Downloader;

	downloader->setUserAgent(m_options.plainUserAgent);

	return downloader;
}

IRobotsTxtLoader* Crawler::createRobotsTxtLoader() const
{
	return new RobotsTxtLoader(new QNetworkAccessManager(const_cast<Crawler*>(this)));
}

SequencedDataCollection* Crawler::sequencedDataCollection() const
{
	if(!m_sequencedDataCollection)
	{
		createSequencedDataCollection(QThread::currentThread());
	}

	return m_sequencedDataCollection.get();
}

QString Crawler::siteMapXml(const SiteMapSettings& settings) const
{
	SiteMap siteMap;
	const SequencedDataCollection* sequencedCollection = sequencedDataCollection();;
	const ISequencedStorage* storage = sequencedCollection->storage(StorageType::CrawledUrlStorageType);
	return siteMap.xml(*storage, settings);
}

void Crawler::saveToFile(const QString& fileName)
{
	QFileInfo fileInfo(fileName);

	const QString fileDirPath = fileInfo.absolutePath();

	const QString baseFilename = fileInfo.completeBaseName();

	const QString fileSuffix = fileInfo.suffix();

	const QString additionalString = QDateTime::currentDateTime().toString(QString("yyyyMMdd_hhmmsszzz"));

	const QString tempFileName = QString("%1/%2_tmp_%3.%4")
		.arg(fileDirPath)
		.arg(baseFilename)
		.arg(additionalString)
		.arg(fileSuffix);

	const QString oldFileName = QString("%1/%2_old_%3.%4")
		.arg(fileDirPath)
		.arg(baseFilename)
		.arg(additionalString)
		.arg(fileSuffix);

	try
	{
		QFile file(tempFileName);

		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			throw std::runtime_error(file.errorString().toStdString());
		}

		Serializer serializer; // TODO: provide all required data into the constructor
		Common::JsonParserStreamWriter writer(file);
		serializer.saveToJsonStream(writer);

		file.close();

		QFile oldFile(fileName);

		const bool oldFileExisits = oldFile.exists();

		if (oldFileExisits)
		{
			if (!oldFile.rename(oldFileName))
			{
				throw std::runtime_error(oldFile.errorString().toStdString());
			}
		}
		QFile::rename(tempFileName, fileName);

		if (oldFileExisits)
		{
			oldFile.remove();
		}
	}
	catch (const std::exception& e)
	{
		Q_UNUSED(e);
		// TODO: notify a user about the error

		return;
	}
}

const UniqueLinkStore* Crawler::uniqueLinkStore() const noexcept
{
	return m_uniqueLinkStore;
}

}

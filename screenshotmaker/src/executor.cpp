#include "executor.h"
#include "ipc_server_channel.h"

namespace ScreenshotMaker
{

Executor::Executor(const QString& pipeChannelName, const QString& sharedMemoryKey, QObject* parent)
	: QObject(parent)
	, m_ipcChannel(new IpcServerChannel(pipeChannelName, this))
	, m_sharedMemory(sharedMemoryKey)
	, m_timer(new QTimer(this))
{
	Q_ASSERT(connect(m_ipcChannel, SIGNAL(screenshotRequested(const QUrl&)), this,
		SLOT(takeScreenshot(const QUrl&)), Qt::QueuedConnection));

	Q_ASSERT(connect(this, SIGNAL(screenshotCreated()), m_ipcChannel,
		SLOT(onScreenshotCreated()), Qt::QueuedConnection));
}

void Executor::takeScreenshot(const QUrl& url)
{
	m_sharedMemory.detach();

	m_timer->setInterval(5000);
	m_timer->setSingleShot(true);

	Q_ASSERT(connect(m_timer, &QTimer::timeout, this, &Executor::onReadyToRenderPixmap));
	Q_ASSERT(connect(m_webEngineView.page(), &QWebEnginePage::loadFinished, this, &Executor::onLoadingDone));

	m_webEngineView.resize(1280, 1024);
	m_webEngineView.load(url);
}

void Executor::onLoadingDone(bool ok)
{
	if (!ok)
	{
		saveScreenshot(QPixmap());
		return;
	}

	m_webEngineView.showMinimized();
	m_timer->start();
}

void Executor::onReadyToRenderPixmap()
{
	QPixmap pixmap = m_webEngineView.grab();
	pixmap = pixmap.scaledToWidth(400, Qt::SmoothTransformation);

	saveScreenshot(pixmap);

	m_webEngineView.close();
}

void Executor::saveScreenshot(const QPixmap& pixmap)
{
	QByteArray pixmapData;
	QBuffer buffer(&pixmapData);
	pixmap.save(&buffer, "png");

	m_sharedMemory.create(pixmapData.size(), QSharedMemory::ReadWrite);

	{
		std::lock_guard locker(m_sharedMemory);
		char* data = static_cast<char*>(m_sharedMemory.data());
		std::copy(pixmapData.begin(), pixmapData.end(), data);
	}

	emit screenshotCreated();
}

}
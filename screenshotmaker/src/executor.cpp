#include "executor.h"
#include "ipc_server_channel.h"

namespace
{

const QSize s_browserWindowSize(1280, 1024);

}

namespace ScreenshotMaker
{

Executor::Executor(const QString& pipeChannelName, const QString& sharedMemoryKey, QObject* parent)
	: QObject(parent)
	, m_webEngineView(nullptr)
	, m_ipcChannel(new IpcServerChannel(pipeChannelName, this))
	, m_sharedMemory(sharedMemoryKey)
	, m_timer(new QTimer(this))
{
	connect(m_ipcChannel, SIGNAL(screenshotRequested(const QUrl&)), this, SLOT(takeScreenshot(const QUrl&)));
	connect(this, SIGNAL(screenshotCreated()), m_ipcChannel, SLOT(onScreenshotCreated()));
}

void Executor::takeScreenshot(const QUrl& url)
{
	if (m_webEngineView)
	{
		m_pendingScreenshotRequests.push(url);
		return;
	}
	else
	{
		m_webEngineView = new QWebEngineView;
		m_webEngineView->setAttribute(Qt::WA_DontShowOnScreen);
		m_webEngineView->page()->settings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
		m_webEngineView->page()->setAudioMuted(true);
	}

	m_sharedMemory.detach();

	m_timer->setInterval(5000);
	m_timer->setSingleShot(true);

	connect(m_timer, &QTimer::timeout, this, &Executor::onReadyToRenderPixmap);
	connect(m_webEngineView->page(), &QWebEnginePage::loadFinished, this, &Executor::onLoadingDone);

	m_webEngineView->resize(s_browserWindowSize);
	m_webEngineView->load(url);
}

void Executor::onLoadingDone(bool ok)
{
	if (!ok)
	{
		saveScreenshot(QPixmap());
		return;
	}

	m_webEngineView->showMinimized();
	m_timer->start();
}

void Executor::onReadyToRenderPixmap()
{
	QPixmap pixmap(s_browserWindowSize);
	QPainter painter(&pixmap);

	m_webEngineView->page()->view()->render(&painter);
	painter.end();

	pixmap = pixmap.scaledToWidth(s_browserWindowSize.width(), Qt::SmoothTransformation);

	saveScreenshot(pixmap);
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

	m_webEngineView->deleteLater();
	m_webEngineView = nullptr;

	doPendingRequests();

	emit screenshotCreated();
}

void Executor::doPendingRequests()
{
	if (m_pendingScreenshotRequests.empty())
	{
		return;
	}

	QUrl url = std::move(m_pendingScreenshotRequests.front());
	m_pendingScreenshotRequests.pop();

	takeScreenshot(url);
}

}
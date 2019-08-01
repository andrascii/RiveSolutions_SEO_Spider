#include "stdafx.h"
#include "executor.h"
#include "message_channel.h"

namespace
{

const QSize s_browserWindowSize(1280, 1024);

}

namespace ScreenshotMaker
{

Executor::Executor(const QString& pipeChannelName, const QString& sharedMemoryKey, QObject* parent)
	: QObject(parent)
	, m_webEngineView(nullptr)
	, m_messageChannel(new MessageChannel(pipeChannelName, this))
	, m_sharedMemory(sharedMemoryKey)
	, m_timer(new QTimer(this))
{
	connect(m_messageChannel, SIGNAL(screenshotRequested(const QUrl&)), this, SLOT(takeScreenshot(const QUrl&)));
	connect(this, SIGNAL(screenshotCreated()), m_messageChannel, SLOT(onScreenshotCreated()));
	connect(m_timer, &QTimer::timeout, this, &Executor::onReadyToRenderPixmap);
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

	connect(m_webEngineView->page(), &QWebEnginePage::loadFinished, this, &Executor::onLoadingDone);

	m_webEngineView->resize(s_browserWindowSize);
	m_webEngineView->load(url);

	qDebug("Loading page started");
}

void Executor::onLoadingDone(bool ok)
{
	if (!ok)
	{
		qDebug("Loading page failed");

		saveScreenshot(QPixmap());
		return;
	}

	qDebug("Page loaded");

	m_webEngineView->showMinimized();
	m_timer->start();
}

void Executor::onReadyToRenderPixmap()
{
	qDebug("Rendering...");

	QPixmap pixmap(s_browserWindowSize);
	QPainter painter(&pixmap);

	m_webEngineView->page()->view()->render(&painter);
	painter.end();

	pixmap = pixmap.scaledToWidth(s_browserWindowSize.width(), Qt::SmoothTransformation);

	qDebug("Screenshot successfully rendered!");

	saveScreenshot(pixmap);
}

void Executor::saveScreenshot(const QPixmap& pixmap)
{
	qDebug("Screenshot saved to shared memory");

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
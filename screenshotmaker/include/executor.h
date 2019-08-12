#pragma once
#include "stdafx.h"

namespace ScreenshotMaker
{

class MessageChannel;

class Executor : public QObject
{
	Q_OBJECT

public:
	Executor(const QString& pipeChannelName, const QString& sharedMemoryKey, QObject* parent = nullptr);

signals:
	void screenshotCreated();

private slots:
	void takeScreenshot(const QUrl& url);
	void onLoadingDone(bool ok);
	void onReadyToRenderPixmap();

private:
	void saveScreenshot(const QPixmap& pixmap);
	void doPendingRequests();

private:
	QWebEngineView* m_webEngineView;
	MessageChannel* m_messageChannel;
	QTimer* m_timer;
	QSharedMemory m_sharedMemory;
	std::queue<QUrl> m_pendingScreenshotRequests;
};

}
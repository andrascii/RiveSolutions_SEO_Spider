#pragma once

#include "ipc_server_wrapper.h"

namespace ScreenshotMaker
{

class IpcServerChannel;

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

private:
	QWebEngineView* m_webEngineView;
	IpcServerChannel* m_ipcChannel;
	QTimer* m_timer;
	QSharedMemory m_sharedMemory;
};

}
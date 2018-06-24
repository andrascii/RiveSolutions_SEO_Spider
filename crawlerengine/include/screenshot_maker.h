#pragma once

#include "iscreenshot_maker.h"
#include "ipc_socket.h"
#include "url.h"

namespace CrawlerEngine
{

class TakeScreenshotRequest;

class ScreenshotMaker : public QObject, public IScreenshotMaker
{
	Q_OBJECT

public:
	ScreenshotMaker(QObject* parent = nullptr);
	~ScreenshotMaker();

	Q_INVOKABLE virtual void handleRequest(RequesterSharedPtr requester) override;
	Q_INVOKABLE virtual void stopRequestHandling(RequesterSharedPtr requester) override;
	virtual QObject* qobject() override;

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void startScreenshotMakerIfNeeded(int attemptsCount = 3);
	void ensureConnection(int attemptsCount = 3);
	void sendScreenshotRequest(const RequesterSharedPtr& requester);
	void logSharedMemoryAttachError();

	void sendTakeScreenshotCommand(TakeScreenshotRequest* request);
	void sendExitCommandToScreenshotMakerProcess();

private:
	std::queue<RequesterSharedPtr> m_requesters;
	RequesterSharedPtr m_currentRequester;
	Common::IpcSocket m_ipcSocket;
	QProcess m_screenshotMakerProcess;
	QSharedMemory m_sharedMemory;
	int m_timerId;
	bool m_isActive;
};

}
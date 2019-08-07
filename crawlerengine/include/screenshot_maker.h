#pragma once

#include "iscreenshot_maker.h"
#include "screenshot_maker_command.h"
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
	bool ensureScreenshotMakerIsAlive(int attemptsCount = 3);
	bool ensureConnection(int attemptsCount = 3);
	void sendScreenshotRequest(const RequesterSharedPtr& requester);
	void logSharedMemoryAttachError();

	void sendTakeScreenshotMessage(TakeScreenshotRequest* request);
	void sendExitCommandToScreenshotMakerProcess();

	void killTimerHelper();
	QPixmap readScreenshotFromMemory();

	std::pair<bool, Common::ScreenshotMakerMessage> screenshotMakerResponse();

private:
	std::queue<RequesterSharedPtr> m_requesters;
	RequesterSharedPtr m_currentRequester;
	Common::IpcSocket m_ipcSocket;
	boost::process::child m_screenshotMakerProcess;
	QSharedMemory m_sharedMemory;
	int m_timerId;
	bool m_isActive;
};

}
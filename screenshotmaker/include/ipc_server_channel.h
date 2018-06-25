#pragma once

#include "ipc_server_wrapper.h"
#include "ipc_command.h"

namespace ScreenshotMaker
{

class IpcServerChannel : public QObject
{
	Q_OBJECT

public:
	IpcServerChannel(const QString& pipeChannelName, QObject* parent = nullptr);

signals:
	void screenshotRequested(const QUrl& url);
	void exitRequested();

public slots:
	void onScreenshotCreated();

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void detectCommandType(const Common::Command& cmd);

private:
	QString m_pipeChannelName;
	Common::IpcServerWrapper m_ipcServer;
	int m_timerId;
};

}
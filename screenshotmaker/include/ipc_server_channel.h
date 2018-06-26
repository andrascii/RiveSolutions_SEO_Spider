#pragma once

#include "ipc_server_wrapper.h"
#include "ipc_command.h"

namespace ScreenshotMaker
{

class PipeConnectionEstablisherThread;

class IpcServerChannel : public QObject
{
	Q_OBJECT

public:
	IpcServerChannel(const QString& pipeChannelName, QObject* parent = nullptr);
	virtual ~IpcServerChannel();

signals:
	void screenshotRequested(const QUrl& url);
	void exitRequested();

public slots:
	void onScreenshotCreated();
	void onConnectionEstablished();

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void detectCommandType(const Common::Command& cmd);

private:
	QString m_pipeChannelName;
	std::shared_ptr<Common::IpcServerWrapper> m_ipcServer;
	PipeConnectionEstablisherThread* m_connectionEstablisherThread;
	int m_timerId;
};

}
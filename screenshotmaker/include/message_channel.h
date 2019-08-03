#pragma once
#include "stdafx.h"

#include "pipe_server.h"
#include "screenshot_maker_command.h"

namespace ScreenshotMaker
{

class PipeConnectionEstablisherThread;

class MessageChannel : public QObject
{
	Q_OBJECT

public:
	MessageChannel(const QString& pipeChannelName, QObject* parent = nullptr);
	virtual ~MessageChannel();

signals:
	void screenshotRequested(const QUrl& url);
	void exitRequested();

public slots:
	void onScreenshotCreated();
	void onConnectionEstablished();

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	void detectCommandType(const Common::ScreenshotMakerMessage& msg);

private:
	QString m_pipeChannelName;
	std::shared_ptr<Common::PipeServer> m_pipeServer;
	PipeConnectionEstablisherThread* m_connectionEstablisherThread;
	int m_timerId;
};

}
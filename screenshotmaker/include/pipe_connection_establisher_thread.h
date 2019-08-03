#pragma once
#include "stdafx.h"

#include "pipe_server.h"

namespace ScreenshotMaker
{

class PipeConnectionEstablisherThread : public QThread
{
	Q_OBJECT

public:
	PipeConnectionEstablisherThread(
		const QString& pipeChannelName,
		const std::shared_ptr<Common::PipeServer>& pipeServer,
		QObject* parent = nullptr
	);

	virtual void run() override;

signals:
	void connectionEstablished();

private:
	const QString& m_pipeChannelName;
	std::weak_ptr<Common::PipeServer> m_pipeServer;
};

}
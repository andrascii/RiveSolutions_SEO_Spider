#pragma once

#include "ipc_server_wrapper.h"

namespace ScreenshotMaker
{

class PipeConnectionEstablisherThread : public QThread
{
	Q_OBJECT

public:
	PipeConnectionEstablisherThread(
		const QString& pipeChannelName,
		const std::shared_ptr<Common::IpcServerWrapper>& ipcServer,
		QObject* parent = nullptr
	);

	virtual void run() override;

signals:
	void connectionEstablished();

private:
	const QString& m_pipeChannelName;
	std::weak_ptr<Common::IpcServerWrapper> m_ipcServer;
};

}
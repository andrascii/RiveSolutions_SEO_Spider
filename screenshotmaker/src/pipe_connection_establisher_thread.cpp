#include "pipe_connection_establisher_thread.h"

namespace ScreenshotMaker
{

PipeConnectionEstablisherThread::PipeConnectionEstablisherThread(
	const QString& pipeChannelName,
	const std::shared_ptr<Common::IpcServerWrapper>& ipcServer,
	QObject* parent)
	: QThread(parent)
	, m_pipeChannelName(pipeChannelName)
	, m_ipcServer(ipcServer)
{
	setTerminationEnabled(true);
}

void PipeConnectionEstablisherThread::run()
{
	std::shared_ptr<Common::IpcServerWrapper> ipcServer = m_ipcServer.lock();

	qDebug("Try listen");

	if (!ipcServer)
	{
		return;
	}

	qDebug("Listen");

	ipcServer->listen(m_pipeChannelName);

	qDebug("New connection");

	emit connectionEstablished();
}

}
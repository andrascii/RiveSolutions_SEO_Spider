#include "stdafx.h"
#include "pipe_connection_establisher_thread.h"

namespace ScreenshotMaker
{

PipeConnectionEstablisherThread::PipeConnectionEstablisherThread(
	const QString& pipeChannelName,
	const std::shared_ptr<Common::PipeServer>& pipeServer,
	QObject* parent)
	: QThread(parent)
	, m_pipeChannelName(pipeChannelName)
	, m_pipeServer(pipeServer)
{
	setTerminationEnabled(true);
}

void PipeConnectionEstablisherThread::run()
{
	std::shared_ptr<Common::PipeServer> pipeServer = m_pipeServer.lock();

	if (!pipeServer)
	{
		qDebug("There is no ipc server instance");
		return;
	}

	qDebug("Listen started");

	pipeServer->listen(m_pipeChannelName);

	qDebug("New connection");

	emit connectionEstablished();
}

}
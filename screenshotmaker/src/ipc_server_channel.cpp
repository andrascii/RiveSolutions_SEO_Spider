#include "ipc_server_channel.h"
#include "pipe_server.h"
#include "ipc_command.h"
#include "common_constants.h"
#include "pipe_connection_establisher_thread.h"

namespace ScreenshotMaker
{

IpcServerChannel::IpcServerChannel(const QString& pipeChannelName, QObject* parent)
	: QObject(parent)
	, m_pipeChannelName(pipeChannelName)
	, m_pipeServer(std::make_shared<Common::PipeServer>())
	, m_connectionEstablisherThread(new PipeConnectionEstablisherThread(m_pipeChannelName, m_pipeServer, this))
	, m_timerId(0)
{
 	connect(m_connectionEstablisherThread, &PipeConnectionEstablisherThread::connectionEstablished,
 		this, &IpcServerChannel::onConnectionEstablished, Qt::QueuedConnection);

	m_connectionEstablisherThread->start();
}

IpcServerChannel::~IpcServerChannel()
{
	if (!m_connectionEstablisherThread->isRunning())
	{
		return;
	}

	m_connectionEstablisherThread->terminate();
	m_connectionEstablisherThread->wait();
}

void IpcServerChannel::onScreenshotCreated()
{
	// send 1 as a signal that we already create a screenshot and sent data using pipe
	const char response = 1;
	m_pipeServer->writeData(response);
	m_pipeServer->closeConnection();

	qDebug("Screenshot created");
}

void IpcServerChannel::onConnectionEstablished()
{
	qDebug("Connection established");

	m_timerId = startTimer(Common::c_minimumRecommendedTimerResolution);
}

void IpcServerChannel::timerEvent(QTimerEvent*)
{
	if (!m_pipeServer->hasConnection())
	{
		qDebug("Has no connection. Kill read timer");

		killTimer(m_timerId);
		m_connectionEstablisherThread->start();
		return;
	}

	Common::ScreenshotCommand cmd;

	if (m_pipeServer->peekData(cmd) != sizeof(cmd))
	{
		return;
	}

	qDebug("Detected screenshot command data");

	const qint64 size = m_pipeServer->readData(cmd);

	if (size < sizeof(Common::ScreenshotCommand))
	{
		qDebug("error reading channel");
		return;
	}

	detectCommandType(cmd);

	if (cmd.type == Common::ScreenshotCommandType::CommandTypeExit)
	{
		killTimer(m_timerId);
	}
}

void IpcServerChannel::detectCommandType(const Common::ScreenshotCommand& cmd)
{
	switch (cmd.type)
	{
		case Common::ScreenshotCommandType::CommandTypeTakeScreenshot:
		{
			qDebug("Screenshot requested");
			emit screenshotRequested(QUrl(cmd.data));
			break;
		}
		case Common::ScreenshotCommandType::CommandTypeExit:
		{
			qDebug("Exit requested");
			emit exitRequested();
			break;
		}
		default:
		{
			Q_ASSERT(!"Invalid command type");
		}
	}
}

}
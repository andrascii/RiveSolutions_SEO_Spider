#include "ipc_server_channel.h"
#include "ipc_server_wrapper.h"
#include "ipc_command.h"
#include "common_constants.h"
#include "pipe_connection_establisher_thread.h"

namespace ScreenshotMaker
{

IpcServerChannel::IpcServerChannel(const QString& pipeChannelName, QObject* parent)
	: QObject(parent)
	, m_pipeChannelName(pipeChannelName)
	, m_ipcServer(std::make_shared<Common::IpcServerWrapper>())
	, m_connectionEstablisherThread(new PipeConnectionEstablisherThread(m_pipeChannelName, m_ipcServer, this))
	, m_timerId(0)
{
	Q_ASSERT(connect(m_connectionEstablisherThread, &PipeConnectionEstablisherThread::connectionEstablished,
		this, &IpcServerChannel::onConnectionEstablished, Qt::QueuedConnection));

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
	m_ipcServer->writeData(response);
	m_ipcServer->closeConnection();
}

void IpcServerChannel::onConnectionEstablished()
{
	m_timerId = startTimer(Common::c_minimumRecommendedTimerResolution);
}

void IpcServerChannel::timerEvent(QTimerEvent*)
{
	if (!m_ipcServer->hasConnection())
	{
		killTimer(m_timerId);
		m_connectionEstablisherThread->start();
		return;
	}

	Common::Command cmd;

	if (m_ipcServer->peekData(cmd) != sizeof(cmd))
	{
		return;
	}

	const qint64 size = m_ipcServer->readData(cmd);

	if (size < sizeof(Common::Command))
	{
		qDebug("error reading channel");
		return;
	}

	detectCommandType(cmd);

	if (cmd.type == Common::CommandType::CommandTypeExit)
	{
		killTimer(m_timerId);
	}
}

void IpcServerChannel::detectCommandType(const Common::Command& cmd)
{
	switch (cmd.type)
	{
		case Common::CommandType::CommandTypeTakeScreenshot:
		{
			emit screenshotRequested(QUrl(cmd.data));
			break;
		}
		case Common::CommandType::CommandTypeExit:
		{
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
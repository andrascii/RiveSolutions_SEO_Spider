#include "ipc_server_channel.h"
#include "ipc_server_wrapper.h"
#include "ipc_command.h"
#include "common_constants.h"

namespace ScreenshotMaker
{

IpcServerChannel::IpcServerChannel(const QString& pipeChannelName, QObject* parent)
	: QObject(parent)
	, m_pipeChannelName(pipeChannelName)
{
	m_timerId = startTimer(Common::c_minimumRecommendedTimerResolution);
}

void IpcServerChannel::onScreenshotCreated()
{
	// send 1 as a signal that we already create a screenshot and sent data using pipe
	const char response = 1;
	m_ipcServer.writeData(response);
	m_ipcServer.closeConnection();
}

void IpcServerChannel::timerEvent(QTimerEvent*)
{
	if (m_ipcServer.hasConnection())
	{
		return;
	}

	m_ipcServer.listen(m_pipeChannelName);

	Common::Command cmd;

	const qint64 size = m_ipcServer.readData(cmd);

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

void IpcServerChannel::detectCommandType(Common::Command cmd)
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
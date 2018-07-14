#include "message_channel.h"
#include "pipe_server.h"
#include "common_constants.h"
#include "pipe_connection_establisher_thread.h"
#include "screenshot_maker_command.h"

using namespace Common;

namespace ScreenshotMaker
{

MessageChannel::MessageChannel(const QString& pipeChannelName, QObject* parent)
	: QObject(parent)
	, m_pipeChannelName(pipeChannelName)
	, m_pipeServer(std::make_shared<Common::PipeServer>())
	, m_connectionEstablisherThread(new PipeConnectionEstablisherThread(m_pipeChannelName, m_pipeServer, this))
	, m_timerId(0)
{
 	connect(m_connectionEstablisherThread, &PipeConnectionEstablisherThread::connectionEstablished,
 		this, &MessageChannel::onConnectionEstablished, Qt::QueuedConnection);

	m_connectionEstablisherThread->start();
}

MessageChannel::~MessageChannel()
{
	if (!m_connectionEstablisherThread->isRunning())
	{
		return;
	}

	m_connectionEstablisherThread->terminate();
	m_connectionEstablisherThread->wait();
}

void MessageChannel::onScreenshotCreated()
{
	ScreenshotMakerMessage msg;
	msg.setResponse(true);

	m_pipeServer->writeData(msg);
	m_pipeServer->closeConnection();

	qDebug("Send response about created screenshot");
}

void MessageChannel::onConnectionEstablished()
{
	qDebug("Connection established");

	m_timerId = startTimer(Common::c_minimumRecommendedTimerResolution);
}

void MessageChannel::timerEvent(QTimerEvent*)
{
	if (!m_pipeServer->hasConnection())
	{
		qDebug("Has no connection. Kill read timer");

		killTimer(m_timerId);
		m_connectionEstablisherThread->start();
		return;
	}

	ScreenshotMakerMessage cmd;

	if (m_pipeServer->peekData(cmd) != sizeof(cmd))
	{
		return;
	}

	qDebug("Read ScreenshotMakerMessage");

	const qint64 size = m_pipeServer->readData(cmd);

	if (size < sizeof(ScreenshotMakerMessage))
	{
		qDebug("error reading channel");
		return;
	}

	detectCommandType(cmd);
}

void MessageChannel::detectCommandType(const ScreenshotMakerMessage& msg)
{
	switch (msg.type)
	{
		case ScreenshotMakerMessage::TypeTakeScreenshot:
		{
			qDebug("Screenshot requested");

			const ScreenshotMakerTakeScreenshot* screenshotMsg = msg.takeScreenshotMessage();
			emit screenshotRequested(QUrl(screenshotMsg->data));

			break;
		}
		case ScreenshotMakerMessage::TypeExit:
		{
			qDebug("Exit requested");
			killTimer(m_timerId);
			break;
		}
		default:
		{
			Q_ASSERT(!"Invalid command type");
		}
	}
}

}
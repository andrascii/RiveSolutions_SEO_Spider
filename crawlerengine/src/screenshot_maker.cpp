#include "screenshot_maker.h"
#include "ipc_command.h"
#include "take_screenshot_request.h"
#include "take_screenshot_response.h"
#include "thread_message_dispatcher.h"
#include "handler_registry.h"
#include "common_constants.h"
#include "url.h"

namespace
{

const QString s_sharedMemoryKey("shared_screenshot_memory");
const QString s_screenshotMakerChannelName("screenshot_maker_channel");

}

namespace CrawlerEngine
{

ScreenshotMaker::ScreenshotMaker(QObject* parent)
	: QObject(parent)
	, m_sharedMemory(s_sharedMemoryKey)
	, m_timerId(0)
	, m_isActive(false)
{
	ensureScreenshotMakerIsAlive();
	ensureConnection();

	HandlerRegistry& handlerRegistry = HandlerRegistry::instance();
	handlerRegistry.registrateHandler(this, RequestType::RequestTakeScreenshot);
}

ScreenshotMaker::~ScreenshotMaker()
{
	sendExitCommandToScreenshotMakerProcess();
}

void ScreenshotMaker::handleRequest(RequesterSharedPtr requester)
{
	if (m_isActive)
	{
		m_requesters.push(requester);
		return;
	}

	sendScreenshotRequest(requester);
}

void ScreenshotMaker::stopRequestHandling(RequesterSharedPtr requester)
{
}

QObject* ScreenshotMaker::qobject()
{
	return this;
}

void ScreenshotMaker::timerEvent(QTimerEvent*)
{
	if (m_ipcSocket.openMode() == QIODevice::NotOpen || m_ipcSocket.isClosed())
	{
		killTimerHelper();
		sendScreenshotRequest(m_currentRequester);
		return;
	}

	if (!isScreenshotReadyForRead())
	{
		return;
	}

	killTimerHelper();

	ThreadMessageDispatcher::forThread(m_currentRequester->thread())->postResponse(
		m_currentRequester, std::make_shared<TakeScreenshotResponse>(readScreenshotFromMemory())
	);

	if (m_requesters.empty())
	{
		m_isActive = false;
		m_currentRequester.reset();
	}
	else
	{
		m_currentRequester = m_requesters.front();
		m_requesters.pop();

		sendScreenshotRequest(m_currentRequester);
	}
}

bool ScreenshotMaker::ensureScreenshotMakerIsAlive(int attemptsCount)
{
	if (m_screenshotMakerProcess.running())
	{
		return true;
	}

	for (int i = 0; i < attemptsCount; ++i)
	{
		m_screenshotMakerProcess = boost::process::child(
			std::string("screenshotmaker.exe " +
				s_screenshotMakerChannelName.toStdString() + " " +
				s_sharedMemoryKey.toStdString() + " " +
				std::to_string(GetCurrentProcessId()))
		);

		if (m_screenshotMakerProcess.running())
		{
			INFOLOG << "screenshotmaker was started";
			break;
		}
	}

	const bool isStarted = m_screenshotMakerProcess.running();

	if (!isStarted)
	{
		ERRLOG << "screenshotmaker was not started";
	}

	return isStarted;
}

bool ScreenshotMaker::ensureConnection(int attemptsCount)
{
	if (m_ipcSocket.openMode() != QIODevice::NotOpen && !m_ipcSocket.isClosed())
	{
		return true;
	}

	// to clear internal state
	m_ipcSocket.disconnectFromServer();

	for (int i = 0; i < attemptsCount; ++i)
	{
		if (m_ipcSocket.connectToServer(s_screenshotMakerChannelName, QIODevice::ReadWrite))
		{
			break;
		}

		std::this_thread::sleep_for(1s);
	}

	return m_ipcSocket.openMode() != QIODevice::NotOpen;
}

void ScreenshotMaker::sendScreenshotRequest(const RequesterSharedPtr& requester)
{
	ASSERT(requester->request()->requestType() == RequestType::RequestTakeScreenshot);

	if (!ensureScreenshotMakerIsAlive())
	{
		ERRLOG << "Cannot start screenshotmaker.exe";
		
		std::shared_ptr<TakeScreenshotResponse> response = std::make_shared<TakeScreenshotResponse>();
		response->setError(ITakeScreenshotResponse::ErrorCannotStartScreenshotMakerProcess);
		
		ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

		return;
	}

	if (!ensureConnection())
	{
		ERRLOG << "Cannot to establish connection with screenshotmaker.exe";

		std::shared_ptr<TakeScreenshotResponse> response = std::make_shared<TakeScreenshotResponse>();
		response->setError(ITakeScreenshotResponse::ErrorCannotEstablishConnection);

		ThreadMessageDispatcher::forThread(requester->thread())->postResponse(requester, response);

		return;
	}

	sendTakeScreenshotCommand(static_cast<TakeScreenshotRequest*>(requester->request()));
	m_currentRequester = requester;

	m_timerId = startTimer(Common::c_minimumRecommendedTimerResolution);
	DEBUG_ASSERT(m_timerId);

	m_isActive = true;
}

void ScreenshotMaker::logSharedMemoryAttachError()
{
	switch (m_sharedMemory.error())
	{
		case QSharedMemory::PermissionDenied:
		{
			ERRLOG << "The operation failed because the caller didn't have the required permissions.";
			break;
		}
		case QSharedMemory::InvalidSize:
		{
			ERRLOG << "A create operation failed because the requested size was invalid.";
			break;
		}
		case QSharedMemory::KeyError:
		{
			ERRLOG << "The operation failed because of an invalid key.";
			break;
		}
		case QSharedMemory::AlreadyExists:
		{
			ERRLOG << "A create() operation failed because a shared memory segment with the specified key already existed.";
			break;
		}
		case QSharedMemory::NotFound:
		{
			ERRLOG << "An attach() failed because a shared memory segment with the specified key could not be found.";
			break;
		}
		case QSharedMemory::LockError:
		{
			ERRLOG << "The attempt to lock() the shared memory segment failed because create() or attach() failed and returned false, or because a system error occurred in QSystemSemaphore::acquire().";
			break;
		}
		case QSharedMemory::OutOfResources:
		{
			ERRLOG << "A create() operation failed because there was not enough memory available to fill the request.";
			break;
		}
		case QSharedMemory::UnknownError:
		{
			ERRLOG << "Something else happened and it was bad.";
			break;
		}
	}
}

void ScreenshotMaker::sendTakeScreenshotCommand(TakeScreenshotRequest* request)
{
	if (m_ipcSocket.openMode() == QIODevice::NotOpen)
	{
		ERRLOG << "There is no connection with screenshotmaker.exe";
		return;
	}

	Common::ScreenshotCommand cmd{ Common::ScreenshotCommandType::CommandTypeTakeScreenshot };
	const std::string url = request->url().toDisplayString().toStdString();
	std::copy(url.begin(), url.end(), cmd.data);
	m_ipcSocket.writeData(reinterpret_cast<const char*>(&cmd), sizeof(cmd));
}

void ScreenshotMaker::sendExitCommandToScreenshotMakerProcess()
{
	if (m_ipcSocket.openMode() == QIODevice::NotOpen)
	{
		ERRLOG << "There is no connection with screenshotmaker.exe";
		return;
	}

	Common::ScreenshotCommand cmd{ Common::ScreenshotCommandType::CommandTypeExit };
	m_ipcSocket.writeData(reinterpret_cast<const char*>(&cmd), sizeof(cmd));
}

void ScreenshotMaker::killTimerHelper()
{
	DEBUG_ASSERT(m_timerId);
	killTimer(m_timerId);
	m_timerId = 0;
}

QPixmap ScreenshotMaker::readScreenshotFromMemory()
{
	if (!m_sharedMemory.attach(QSharedMemory::ReadOnly))
	{
		logSharedMemoryAttachError();

		ERRLOG << "Cannot attach to shared memory to read the screenshot image data";
	}

	QPixmap pixmap;

	{
		std::lock_guard locker(m_sharedMemory);
		uchar* imageData = static_cast<uchar*>(m_sharedMemory.data());
		pixmap.loadFromData(imageData, m_sharedMemory.size());
	}

	m_sharedMemory.detach();

	return pixmap;
}

bool ScreenshotMaker::isScreenshotReadyForRead()
{
	char response = 0;

	// wait 1 byte then read this and check value
	if (m_ipcSocket.peekData(&response, sizeof(response)) != 1)
	{
		return false;
	}
	else
	{
		m_ipcSocket.readData(&response, sizeof(response));
	}

	return response == 1;
}

}
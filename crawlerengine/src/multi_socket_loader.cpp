#include "multi_socket_loader.h"

namespace CrawlerEngine
{

GlobalInfo MultiSocketLoader::s_globalInfo;

MultiSocketLoader::MultiSocketLoader(QObject* parent)
	: QObject(parent)
	, m_timer(nullptr)
{
	qRegisterMetaType<SocketInfo>("SocketInfo");

	s_globalInfo.multi = curl_multi_init();

	ASSERT(s_globalInfo.multi);

	curl_multi_setopt(s_globalInfo.multi, CURLMOPT_SOCKETFUNCTION, socketFunctionCallback);
	curl_multi_setopt(s_globalInfo.multi, CURLMOPT_SOCKETDATA, this);
	curl_multi_setopt(s_globalInfo.multi, CURLMOPT_TIMERFUNCTION, multiTimerCallback);
	curl_multi_setopt(s_globalInfo.multi, CURLMOPT_TIMERDATA, this);
}

void MultiSocketLoader::timeout()
{
	onTimeout();
}

void MultiSocketLoader::get(const QByteArray& url)
{
	ConnectionInfo* connection = new ConnectionInfo;
	connection->easy = curl_easy_init();
	connection->url = url;

	curl_easy_setopt(connection->easy, CURLOPT_CONNECTTIMEOUT, 30);
	curl_easy_setopt(connection->easy, CURLOPT_URL, url.constData());
	curl_easy_setopt(connection->easy, CURLOPT_PRIVATE, connection);
	curl_easy_setopt(connection->easy, CURLOPT_ERRORBUFFER, connection->error);
	curl_easy_setopt(connection->easy, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(connection->easy, CURLOPT_WRITEFUNCTION, &writeBodyCallback);
	curl_easy_setopt(connection->easy, CURLOPT_WRITEDATA, connection);
	curl_easy_setopt(connection->easy, CURLOPT_SSL_VERIFYPEER, 0);

	curl_multi_add_handle(s_globalInfo.multi, connection->easy);
}

void MultiSocketLoader::checkMultiInfo()
{
	CURLMsg* msg;
	int messagesLeft;

	while ((msg = curl_multi_info_read(s_globalInfo.multi, &messagesLeft)))
	{
		if (msg->msg == CURLMSG_DONE)
		{
			CURL* easy = msg->easy_handle;
			CURLcode result = msg->data.result;

			ConnectionInfo* connection = nullptr;
			curl_easy_getinfo(easy, CURLINFO_PRIVATE, &connection);

			if (result != CURLE_OK)
			{
				ERRLOG << connection->error;
			}
			else
			{
				DEBUGLOG << connection->url << "loaded";
			}

			curl_multi_remove_handle(s_globalInfo.multi, easy);
			curl_easy_cleanup(easy);

			delete connection;
		}
	}
}

void MultiSocketLoader::onTimeout()
{
	CURLMcode returnCode = curl_multi_socket_action(s_globalInfo.multi, CURL_SOCKET_TIMEOUT, 0, &s_globalInfo.stillRunning);
	ASSERT(returnCode == CURLE_OK);
	checkMultiInfo();
}

void MultiSocketLoader::setTimer()
{
	resetTimer();
	m_timer = new QTimer(this);
	m_timer->setInterval(10000);
	m_timer->setSingleShot(true);

	VERIFY(connect(m_timer, &QTimer::timeout, this, &MultiSocketLoader::timeout));

	m_timer->start();
}

void MultiSocketLoader::resetTimer()
{
	if (m_timer && m_timer->isActive())
	{
		m_timer->stop();
		m_timer->deleteLater();
		m_timer = nullptr;
	}
}

size_t MultiSocketLoader::writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata)
{
	INFOLOG << "write";
	ConnectionInfo* connectionInfo = reinterpret_cast<ConnectionInfo*>(userdata);
	const auto bufferSize = size * nmemb;
	connectionInfo->data.append(reinterpret_cast<const char*>(buffer), static_cast<int>(bufferSize));
	return bufferSize;
}

int MultiSocketLoader::socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData)
{
	const auto setSocketInfo = [&](SocketInfo* socketInfo)
	{
		socketInfo->sockfd = s;
		socketInfo->action = action;
		socketInfo->easy = easy;
	};

	MultiSocketLoader* loader = reinterpret_cast<MultiSocketLoader*>(userPrivateData);
	SocketInfo* socketInfo = reinterpret_cast<SocketInfo*>(socketPrivateData);

	if (action == CURL_POLL_REMOVE)
	{
		delete socketInfo;
	}
	else
	{
		if (!socketInfo)
		{
			socketInfo = new SocketInfo;
			setSocketInfo(socketInfo);
			curl_multi_assign(s_globalInfo.multi, s, socketInfo);
		}
		else
		{
			setSocketInfo(socketInfo);
		}

		loader->resetTimer();

		QMetaObject::invokeMethod(loader, "doAction", Qt::QueuedConnection, Q_ARG(const SocketInfo&, *socketInfo));
	}

	return 0;
}

int MultiSocketLoader::multiTimerCallback(CURLM*, long timeoutMs, void* userPrivateData)
{
	MultiSocketLoader* loader = reinterpret_cast<MultiSocketLoader*>(userPrivateData);

	loader->resetTimer();

	if (timeoutMs > 0)
	{
		loader->setTimer();
	}
	else
	{
		onTimeout();
	}

	return 0;
}

void MultiSocketLoader::doAction(const SocketInfo& socketInfo)
{
	CURLMcode rc;
	rc = curl_multi_socket_action(s_globalInfo.multi, socketInfo.sockfd, socketInfo.action, &s_globalInfo.stillRunning);

	checkMultiInfo();

	if (s_globalInfo.stillRunning <= 0)
	{
		resetTimer();
	}
	else
	{
		QMetaObject::invokeMethod(this, "doAction", Qt::QueuedConnection, Q_ARG(const SocketInfo&, socketInfo));
	}
}

}

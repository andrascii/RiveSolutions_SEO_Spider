#pragma once

namespace CrawlerEngine
{

/* Global information, common to all connections */
struct GlobalInfo
{
	CURLM* multi;
	int stillRunning;
};

/* Information associated with a specific easy handle */
struct ConnectionInfo
{
	CURL* easy;
	QByteArray url;
	char error[CURL_ERROR_SIZE];
	QByteArray data;
	GlobalInfo* globalInfo;
};

struct SocketInfo
{
	curl_socket_t sockfd;
	CURL* easy;
	int action;
	long timeout;
};

class MultiSocketLoader : public QObject
{
	Q_OBJECT

public:
	MultiSocketLoader(QObject* parent = nullptr);

	void get(const QByteArray& url);

private slots:
	void timeout();
	void doAction(const SocketInfo& socketInfo);

private:
	void setTimer();
	void resetTimer();

private:
	static void checkMultiInfo();
	static void onTimeout();
	static void removeSocket();

	static size_t writeBodyCallback(void* buffer, size_t size, size_t nmemb, void* userdata);
	static int socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData);
	static int multiTimerCallback(CURLM* multi, long timeoutMs, void* userPrivateData);

private:
	static GlobalInfo s_globalInfo;

private:
	QTimer* m_timer;
};

Q_DECLARE_METATYPE(SocketInfo)

}
#pragma once

namespace CrawlerEngine
{

class HopsChain;

class MultiSocketLoader : public QObject
{
	Q_OBJECT

public:
	MultiSocketLoader(QObject* parent = nullptr);
	~MultiSocketLoader();

	void get(const QByteArray& url);
	void post(const QByteArray& url);
	void head(const QByteArray& url);

signals:
	void done(const HopsChain& hopsChain);

private slots:
	void timeout();
	void doAction(curl_socket_t socketDescriptor, int curlAction);

private:
	void setTimer();
	void resetTimer();

private:
	struct SocketFunctionCallbackPrivateData
	{
		MultiSocketLoader* multiSocketLoader;
		CURLM* multiHandle;
		int stillRunning;
	};

	static int socketFunctionCallback(CURL* easy, curl_socket_t s, int action, void* userPrivateData, void* socketPrivateData);
	static int multiTimerCallback(CURLM* multi, long timeoutMs, void* userPrivateData);

private:
	QTimer* m_timer;
	SocketFunctionCallbackPrivateData m_socketPrivateData;
};

}
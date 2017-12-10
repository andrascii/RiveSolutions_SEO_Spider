#pragma once

namespace SeoSpider
{

class InternetConnectionInspector : public QObject
{
	Q_OBJECT

private:
	friend class InternetConnectionNotificationManager;

	static InternetConnectionInspector* instance();

	static void init();
	static void term();

private slots:
	void start();
	void stop();

signals:
	void statusChanged(bool);

protected:
	virtual void timerEvent(QTimerEvent* event) override;

private:
	InternetConnectionInspector(QObject* parent = nullptr);
	~InternetConnectionInspector();

	bool doPing() const noexcept;

private:
	QStringList m_hostList;

	std::string_view m_sendData;

	std::size_t m_replyBufferSize;
	std::unique_ptr<char[]> m_replyBuffer;

	int m_timerId;

	bool m_internetAvailable;
};

}
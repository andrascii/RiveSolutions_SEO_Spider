#pragma once

namespace SeoSpider
{

class InternetConnectionInspector : public QObject
{
	Q_OBJECT

public:
	static InternetConnectionInspector* instance();

	static void init();
	static void term();

	bool internetAvailable() const noexcept;

	Q_SLOT void start();
	Q_SLOT void stop();

signals:
	void statusChanged();

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
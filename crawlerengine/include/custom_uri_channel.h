#pragma once

namespace CrawlerEngine
{
	
class CustomUrlChannel: public QObject
{
	Q_OBJECT

public:
	static CustomUrlChannel* instance();
	static bool writeUri(const QByteArray& uri);
	virtual ~CustomUrlChannel();
	Q_SIGNAL void uriReceived(const QByteArray& uri);

protected:
	virtual void timerEvent(QTimerEvent*) override;

private:
	CustomUrlChannel(QObject* parent, bool host);

private:
	QSharedMemory m_sharedMemory;
	bool m_host;
	char m_counter;
};

}
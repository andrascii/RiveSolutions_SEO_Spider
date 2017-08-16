#pragma once

namespace WebCrawler
{

class NamedThread : public QThread
{
public:
	NamedThread(const QByteArray& name, QObject* parent = NULL);

protected:
	void run() override;
};

}
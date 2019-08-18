#pragma once

namespace Common
{

class NamedThread : public QThread
{
public:
	NamedThread(const QByteArray& name, QObject* parent = NULL);

protected:
	void run() override;
};

}
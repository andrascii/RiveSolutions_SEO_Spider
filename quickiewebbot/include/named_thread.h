#pragma once


#pragma once

namespace QuickieWebBot
{

class NamedThread : public QThread
{
public:
	NamedThread(const QByteArray& name, QObject* parent = NULL);

protected:
	void run() override;
};

}
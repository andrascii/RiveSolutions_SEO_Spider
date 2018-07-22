#pragma once

#include "ipc_socket.h"

namespace Common
{

struct Command;

}

namespace SeoSpiderService
{

class CommandThread : public QThread
{
	Q_OBJECT

public:
	CommandThread(Common::IpcSocket* socket, const QString& file);

	virtual void run() override;

signals:
	void commandReceived(const Common::Command& command);

private:
	QVariantMap m_statisticsJsonHeader;

	Common::IpcSocket* m_socket;
	QFile m_outputFile;
};

}
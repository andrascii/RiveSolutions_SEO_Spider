#pragma once

#include "ipc_socket.h"

namespace Common
{

struct PipeMessage;

}

namespace SeoSpiderService
{

class LogThread : public QThread
{
	Q_OBJECT

public:
	LogThread(Common::IpcSocket* socket, const QString& file);

	virtual void run() override;

signals:
	void messageReceived(const Common::PipeMessage& message);

private:
	Common::IpcSocket* m_socket;
	QFile m_outputFile;
};

}
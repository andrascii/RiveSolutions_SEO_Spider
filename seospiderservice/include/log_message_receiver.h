#pragma once

#include "ilogger.h"

namespace SeoSpiderService
{

struct Message
{
    std::uint64_t messageSize;
    SeoSpiderServiceApi::SeverityLevel severityLevel;
    QString message;
};

class LogMessageReceiver : public QObject
{
    Q_OBJECT

public:
    LogMessageReceiver(QObject* parent = nullptr);

    Q_SIGNAL void messageReceived(Message msg);

private:
    Q_SLOT void onNewConnection();
    Q_SLOT void readMessage();
    Q_SLOT void onConnectionClosed();

private:
    QLocalServer* m_server;
    QLocalSocket* m_currentConnectionSocket;
    Message m_message;
};

}

Q_DECLARE_METATYPE(SeoSpiderService::Message)